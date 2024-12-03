import itertools
import collections
import json
from enum import Enum

def validate_int(i):
    try:
        return int(i)
    except (ValueError, TypeError):
        return 0

def validate_char(s):
    try:
        return str(s)
    except (ValueError, TypeError):
        return '0'

class TypeTag(object):
    '''Wrapper for a primitive type'''
    kind = "primitive"

    def __init__(self, label, validator=None):
        self.label = label
        self.validator = validator

    @property
    def typename(self):
        return self.label

    @property
    def ident_label(self):
        return "IDENT_" + self.label

    @property
    def types_referenced(self):
        return []

    @property
    def default(self):
        return 0

    def children(self):
        return []

    def is_collection(self):
        return False

    def is_primitive(self):
        return True

    def validate(self, obj):
        if self.validator:
            return self.validator(obj)
        return obj

    def generate_js_schema(self, label=None):
        data = {
            "kind": "primitive",
            "type": self.label,
            "value": 0
        }
        if self.label is None:
            data["label"] = label

        return data

class PrimitiveTypes(Enum):
    '''Native primitive types'''
    CHAR   = TypeTag("char",     validate_char)
    INT    = TypeTag("int",      validate_int)
    LONG   = TypeTag("long",     validate_int)
    SIZE_T = TypeTag("size_t",   validate_int)
    UINT8  = TypeTag("uint8_t",  validate_int)
    INT8   = TypeTag("int8_t",   validate_int)
    UINT16 = TypeTag("uint16_t", validate_int)
    INT16  = TypeTag("int16_t",  validate_int)
    UINT32 = TypeTag("uint32_t", validate_int)
    INT32  = TypeTag("int32_t",  validate_int)
    UINT64 = TypeTag("uint64_t", validate_int)
    INT64  = TypeTag("int64_t",  validate_int)

    FLOAT  = TypeTag("float")
    DOUBLE = TypeTag("double")

class Type(object):
    '''Primitive types, with a validation/default function'''
    kind = "primitive"

    def __init__(self, tag, validator=None, default=None):
        self.tag = tag
        self._typename = tag.label
        self.validator = validator
        self._default = default

    @property
    def typename(self):
        return self._typename

    @property
    def ident_label(self):
        return "IDENT_" + self._typename

    @property
    def types_referenced(self):
        return []

    def children(self):
        return []

    def is_collection(self):
        return False

    def is_primitive(self):
        return True

    @property
    def default(self):
        return self.validate(self._default)

    def validate(self, obj):
        obj = self.tag.validate(obj)
        if self.validator:
            return self.validator(obj)
        return obj

    def generate_js_schema(self, label=None):
        data = {
            "kind": "primitive",
            "type": self._typename,
            "value": self._default or 0
        }
        if label is not None:
            data["label"] = label

        return data

def fields(**kwargs):
    return collections.OrderedDict(kwargs)

class Struct(object):
    '''Schema object for a struct: a struct typename, and its fields

    Defaults are a table {fieldname: value}'''
    kind = "struct"

    def __init__(self, name, fields, default=None):
        self.name = name
        self.fields = fields
        self.default = default or {}
        self.instance_label = None # set up during label assignment

    @property
    def typename(self):
        return "struct %s" % (self.name,)

    @property
    def refname(self):
        return "struct %s *" % (self.name,)

    def is_collection(self):
        return True

    def is_primitive(self):
        return False

    def children(self):
        return list(self.fields.values())

    @property
    def ident_label(self):
        return "IDENT_STRUCT_" + self.name

    @property
    def types_referenced(self):
        return []

    @property
    def accessor_label(self):
        return "ACCESSOR_STRUCT_" + self.name
    
    def generate_field(self, fieldname, field):
        if field.is_primitive():
            return "    %s %s;" % (field.typename, fieldname,)
        else:
            return "    const %s%s;" % (field.refname, fieldname,)

    def generate_fields(self):
        strs = []
        for fieldname, field in self.fields.items():
            strs.append("\n" + self.generate_field(fieldname, field))
        strs.append("\n")
        return "".join(strs)

    def generate_type_definition(self):
        return "%s {" % (self.typename,) + self.generate_fields() + "};"

    def generate_init_definition(self, result, idx=0):
        initializers = []
        for fieldname, field in self.fields.items():
            if field.is_primitive():
                if fieldname in self.default:
                    initializers.append(str(field.validate(self.default[fieldname])))
                else:
                    initializers.append(str(field.default))
            else:
                result, idx = field.generate_init_definition(result, idx)
                initializers.append("&" + field.instance_label)

        self.instance_label = "id_%d" % idx
        dn = "%s %s = {%s};"  % (self.typename, self.instance_label, ", ".join(initializers))

        return result + "\n" + dn + "\n", idx + 1

    def field_accessor_enum(self, fieldname):
        return self.accessor_label + "_" + fieldname

    def generate_accessor_enum(self):
        comment = "// Member accessor for %s %s" % (self.typename, self.instance_label,)
        tag = "ACCESSOR_STRUCT_" + self.name
        accessor_types = [self.field_accessor_enum(fieldname) for fieldname in self.fields.keys()]
        return comment + "\nenum " + tag + " {\n    " + ",\n    ".join(accessor_types) + "\n};"

    def generate_accessor(self):
        '''Generate pointer/type label retrievals for every field'''
        case_begin  = ""
        case_begin += "case %s: // accessing struct %s\n" % (self.ident_label, self.name,)
        case_begin += "    switch (member) {\n"

        case_end    = "    default:\n"
        case_end   += "        goto broken_accessor;\n"
        case_end   += "    }\n"
        case_end   += "    break;\n"

        accessor_cases = []
        for fieldname, field in self.fields.items():
            case = ""
            if field.is_primitive():
                case += "    case %s:\n" % (self.field_accessor_enum(fieldname))
                case += "        treeref = (void *) &((%s) treeref)->%s;\n" % (self.refname, fieldname,)
                case += "        ident = %s;\n" % (field.ident_label)
                case += "        break;\n"
            else: # collection type
                case += "    case %s:\n" % (self.field_accessor_enum(fieldname))
                case += "        treeref = (void *) ((%s) treeref)->%s;\n" % (self.refname, fieldname,)
                case += "        ident = %s;\n" % (field.ident_label)
                case += "        break;\n"
            accessor_cases.append(case)

        return case_begin + "".join(accessor_cases) + case_end

    def validate(self, obj):
        schema_keys = set(self.fields.keys())
        instance_keys = set(obj.keys())
    
        missing = []
        extra = []

        for fieldname, field in self.fields.items():
            if fieldname in obj:
                field.validate(obj[fieldname])
            else:
                missing.append(field)

        if missing:
            raise ValueError("Missing fields: " + str(missing))

        extra.extend(instance_keys.difference(schema_keys))
        if extra:
            raise ValueError("Extra fields: " + str(extra))

    def generate_js_schema(self, label=None):
        fields = []
        children = []
        for idx, (name, field_type) in enumerate(self.fields.items()):
            schema = field_type.generate_js_schema(label=name)
            schema["_accessor"] = idx
            if field_type.is_primitive():
                fields.append(schema)
            else:
                children.append(schema)
        return {
            "label": label,
            "kind": "struct",
            "fields": fields,
            "children": children,
        }

class List(object):
    '''Schema type for lists: a name, element type, and capacity'''
    kind = "list"

    def __init__(self, name, typ, capacity=None, default=None):
        assert capacity is not None
        self.name = name
        self.member_type = typ
        self.member_typename = self.member_type.typename.replace(" ", "_")
        self.capacity = capacity
        self._default = default

        self.instance_label = None

    @property
    def typename(self):
        return "struct %s" % (self.name,)

    @property
    def refname(self):
        return "struct %s *" % (self.name,)

    def children(self):
        return [self.member_type]

    @property
    def ident_label(self):
        return "IDENT_LIST_" + self.member_typename

    @property
    def types_referenced(self):
        return [PrimitiveTypes.UINT16.value]

    @property
    def length_accessor_label(self):
        return "ACCESSOR_LIST_GENERIC_length"

    def generate_accessor_enum(self):
        return """enum ACCESSOR_LIST_GENERIC {
    ACCESSOR_LIST_GENERIC_capacity,
    ACCESSOR_LIST_GENERIC_length,
    ACCESSOR_LIST_GENERIC_type
};"""
    def is_collection(self):
        return True

    def is_primitive(self):
        return False

    def generate_type_definition(self):
        member_typename = self.member_typename + " " if self.member_type.is_primitive() else self.member_type.refname
        return """
%s {
    const uint16_t capacity;
    uint16_t length;
    %s%slist[%d];
};""" % (self.typename, "const " if not self.member_type.is_primitive() else "", member_typename, self.capacity,)

    def generate_init_definition(self, result, idx=0):
        length = self.capacity
        initializer = []
        if self.member_type.is_primitive():
            if self._default:
                length = len(self._default)
                initializer = [str(self.member_type.validate(val)) for val in self._default]
            else:
                initializer = ["0"] * length
        else:
            # collection type -- visit first to generate all the space needed for the sub-type
            for x in range(self.capacity):
                result, idx = self.member_type.generate_init_definition(result, idx)
                initializer.append("&" + self.member_type.instance_label)

        self.instance_label = "id_%d" % idx
        dn = "%s %s = {%d, %d, {%s}};" % (self.typename, self.instance_label,
                                          self.capacity, length,
                                          ", ".join(initializer),)
        return result + "\n" + dn + "\n", idx + 1

    def validate(self, obj):
        for o in obj:
            self.member_type.validate(o)

    def generate_accessor(self):
        case_begin  = ""
        case_begin += "case %s: // accessing list [%s]" % (self.ident_label, self.member_typename,)
        case_begin += "\n"
        case_begin += "    switch (member) {"
        case_begin += "\n"

        capacity_case  = "    case (ACCESSOR_LIST_GENERIC_capacity):\n"
        capacity_case += "        if (access == accessor_type_write)\n"
        capacity_case += "            *status = status_code_write_protected;\n"
        capacity_case += "        treeref = (void *) &((%s) treeref)->capacity;\n" % (self.refname,)
        capacity_case += "        ident = IDENT_uint16_t;\n"
        capacity_case += "        break;\n"

        length_case  = "    case (ACCESSOR_LIST_GENERIC_length):\n"
        length_case += "        treeref = (void *) &((%s) treeref)->length;\n" % (self.refname,)
        length_case += "        ident = IDENT_uint16_t;\n"
        length_case += "        break;\n"

        type_case    = "    case (ACCESSOR_LIST_GENERIC_type):\n"
        type_case   += "        ident = %s;\n" % (self.member_type.ident_label,)
        type_case   += "        break;\n"

        index_case   = "    default:\n"
        index_case  += "        if (member - 3 >= ((%s) treeref)->length) {\n" % (self.refname,)
        index_case  += "            *status = status_code_out_of_bounds;\n"
        index_case  += "            return;\n"
        index_case  += "        }\n"
        if self.member_type.is_primitive():
            index_case  += "        treeref = (void *) &((%s) treeref)->list[member - 3];\n" % (self.refname,)
            index_case  += "        ident = %s;\n" % (self.member_type.ident_label,)
        else:
            index_case  += "        treeref = (void *) ((%s) treeref)->list[member - 3];\n" % (self.refname,)
            index_case  += "        ident = %s;\n" % (self.member_type.ident_label,)

        case_end  = "    }"
        case_end += "\n"
        case_end += "    break;"

        return case_begin + capacity_case + length_case + type_case + index_case + case_end

    def generate_js_schema(self, label=None):
        data = {
            "kind": "list",
            "capacity": self.capacity,
            "type": self.member_type.generate_js_schema(),
            "items": [],
        }
        if label is not None:
            data["label"] = label
        return data

def typenode_iter(node):
    '''post-order traversal of the node tree'''
    for childnode in node.children():
        yield from typenode_iter(childnode)
    yield node

def generate_type_definitions(root_struct):
    '''config.h type interface'''
    seen = set()
    result = ""
    for node in typenode_iter(root_struct):
        if node.is_collection() and node not in seen:
            result += node.generate_type_definition() + "\n"
            seen.add(node)
    return result

def generate_idents(root_struct):
    '''Enumerated type identifiers, and their bins and sizes'''
    idents = {}
    for node in typenode_iter(root_struct):
        for typenode in [node] + node.types_referenced:
            idents[typenode.typename] = typenode

    kinds = ["primitive", "list", "struct"]
    kind_bins = [[], [], []]

    for node in idents.values():
        if node.kind == "primitive":
            kind_bins[0].append(node)
        elif node.kind == "list":
            kind_bins[1].append(node)
        elif node.kind == "struct":
            kind_bins[2].append(node)

    invalid_size = "    -1"
    ident_entries = []
    sizes_entries = []

    for kind_label, kind_bin in zip(kinds, kind_bins):
        for node in kind_bin:
            ident_entries.append(node.ident_label)
            sizes_entries.append("    sizeof(%s)" % (node.typename))
        ident_entries.append("IDENT__END_OF_" + kind_label.upper() + "S")
        sizes_entries.append(invalid_size)

    result = ""
    result += "enum ident_type {\n"
    result += ",\n".join(["    " + ident for ident in ident_entries])
    result += "\n};\n\n"

    result += "const int ident_sizes[] = {\n"
    result += ",\n".join(sizes_entries)
    result += "\n};\n\n"

    result += "const char *ident_labels[] = {\n"
    result += ",\n".join(['    "%s"' % (ident) for ident in ident_entries])
    result += "\n};\n"

    result += """
#define ident_is_primitive(ident) ((ident) >= 0 && (ident) < IDENT__END_OF_PRIMITIVES)
#define ident_is_list(ident)      ((ident) > IDENT__END_OF_PRIMITIVES && (ident) < IDENT__END_OF_LISTS)
#define ident_is_struct(ident)    ((ident) > IDENT__END_OF_LISTS && (ident) < IDENT__END_OF_STRUCTS)
"""
    return result

def generate_accessor_enums(root_struct):
    '''Member lookups for each collection type

    Note that lists share a generic accessor enum'''
    result = """
/* ACCESSOR AND TYPE LABELS */
"""
    seen_list = False
    for node in typenode_iter(root_struct):
        if node.is_collection():
            if node.kind == "list":
                if seen_list:
                    continue
                seen_list = True
            result += node.generate_accessor_enum() + "\n\n"
    return result

def max_depth(node):
    return 1 + max([0] + [max_depth(child) for child in node.children() if child.is_collection()])

def generate_instance_fixtures(root_struct):
    '''Fixtures needed for reference in the hand-written library'''
    return """
#define ROOT_STRUCT_REF &%s
#define ROOT_STRUCT_IDENT %s
#define ROOT_STRUCT_TYPE %s
#define ROOT_STRUCT_TYPE_PTR ROOT_STRUCT_TYPE *
#define MAX_DEPTH %d
""" % (root_struct.instance_label,
       root_struct.ident_label,
       root_struct.typename,
       max_depth(root_struct),)

def accessor_types():
    return """
enum accessor_type {
    accessor_type_read,
    accessor_type_write,
    accessor_type_scan,
    accessor_last
};
"""

def status_codes():
    return """
// protocol status codes
enum status_code {
    status_code_ok,
    status_code_out_of_bounds,
    status_code_write_protected,
    status_code_invalid_accessor,
    status_code_invalid_type,
    status_code_broken_ptr,
    status_code_invalid_length,
    status_code_length_overflow,
    status_code_length_mismatch,
    status_code_invalid_opcode,
    status_code_broken_packet = 11,
    status_code_need_sync,
    status_code_no_trx,
    status_code_timeout
};
"""

def generate_accessor(root_struct):
    preamble = """
/* From the pointer treeref of type ident, access the given member into value
 * and value_type. */
void accessor(void *treeref, uint8_t ident, uint8_t member, enum accessor_type access,
              void **value, uint8_t *value_type, enum status_code *status) {
    if (!treeref)
        goto broken_ptr;

    *status = status_code_ok;
    switch (ident) {
"""
    end = """
    default:
        goto broken_ident_type;
    }
    *value = treeref;
    *value_type = ident;
    return;

broken_accessor:
    *status = status_code_invalid_accessor;
    return;
broken_ident_type:
    *status = status_code_invalid_type;
    return;
broken_ptr:
    *status = status_code_broken_ptr;
    return;
}
"""
    seen_list = False # need only one list accessor
    accessor_cases = []
    for node in typenode_iter(root_struct):    
        if node.is_collection():
            if node.kind == "list":
                if seen_list:
                    continue
                else:
                    seen_list = True
            accessor = node.generate_accessor()
            accessor = "    " + accessor.replace("\n", "\n    ")
            accessor_cases.append(accessor)

    return preamble + "\n".join(accessor_cases) + end

def beginning():
    return """#include "config.h"
#include <stdlib.h>
#include <FlashAsEEPROM.h>

// Send a debug message over the wire
#define proto_debug 1
void proto_print(String str);
"""

def generate_init(root_struct):
    result = """
/* INSTANCE */
"""
    return root_struct.generate_init_definition(result)

def generate_config_header(root_struct):
    header = """#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

"""
    header += generate_type_definitions(root_struct)
    header += """
void init_config();

%s read_config();

/* Commit the current config to storage
 *
 * Use this commit function to persist external edits made to the config tree;
 * config updates handled internally are automatically committed.
 */
void commit_config();

uint8_t current_slot();

const uint8_t slots = 2;

void set_slot(uint8_t slot);

/* Commit the current config into the store */
void store_config();

/* Drop into checking for a serial update */
bool handle_serial_update();
""" % (root_struct.refname,)

    return header

def generate_library_file(root_struct):
    result = ""
    result += beginning()
    result += generate_init(root_struct)[0]
    result += generate_accessor_enums(root_struct)
    result += generate_idents(root_struct)
    result += generate_instance_fixtures(root_struct)
    result += accessor_types()
    result += status_codes()
    result += generate_accessor(root_struct)
    return result

def make_arduino_library(root_config):
    with open("config.h", "w") as f:
        print(generate_config_header(root_config), file=f)
    with open("ardconfig.cpp", "w") as f:
        print(generate_library_file(root_config), file=f)
        with open("arduino_src/ardlib.c.part", "r") as fread:
            f.write(fread.read())

def make_js_schema(root_struct):
    with open("config.json", "w", encoding='utf-8') as f:
        json.dump(root_struct.generate_js_schema("root"), f, indent=4)
