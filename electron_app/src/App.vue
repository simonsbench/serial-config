<template>
  <div class="mainPanel">
    <div class="topStrip">
      <div class="portScan">
        <div class="portScanButton">
          <button v-on:click="get_ports" class="button">
            <h3 class="buttonText">scan</h3>
          </button>
        </div>
        <div class="portSelectionDropdown">
          <select port @change="selected_port($event)">
            <option v-for="(portdef, index) in ports" :value="index"> {{ portdef.path }} </option>
          </select>
        </div>
      </div>
      <div class="filler"/>
      <div class="buttons">
        <div class="resetButton">
          <button v-on:click="reset_to_device" class="button">
            <h3 class="buttonText">reset</h3>
          </button>
        </div>
        <div class="readButton">
          <button v-on:click="schema_read" class="button">
            <h3 class="buttonText">read</h3>
          </button>
        </div>
        <div class="updateButton">
          <button v-on:click="schema_update" class="button">
            <h3 class="buttonText">sync</h3>
          </button>
        </div>
      </div>
    </div>
    <div id="panel" class="editPanel">
    </div>
    <div class="feedbackStrip">
      <p> feedback: {{ console_feedback }} </p>
    </div>
  </div>
  <div class="hierarchyNavigator">
    <SchemaNode :node="schema"
                    :level="0"
                    :ctx="ctx"
                    @selected_node="selected"
                    @list_element_edited="list_element_edited"
                    @list_as_string_edited="list_as_string_edited"
                    @list_element_added="list_element_added"
                    @list_element_up="list_element_up"
                    @list_element_down="list_element_down"
                    @list_element_delete="list_element_delete"
                    @struct_field_edited="struct_field_edited"/>
  </div>
</template>

<script>
import { ref } from 'vue';
import SchemaNode from './components/SchemaNode.vue';
import DisplayPort from './components/DisplayPort.vue';

/* ------------------------
   Validation of data types
   ------------------------ */
function validate_integer(value, definition, uid, args) {
  var int_value;
  try {
    int_value = BigInt(value);
  } catch {
    return [value, false, "Unable to parse value into type: " + definition.type]
  }

  const bits = definition._bytec || args[0];
  const is_signed = args[1];

  if (is_signed) {
    int_value = BigInt.asIntN(bits, int_value);
  } else {
    int_value = BigInt.asUintN(bits, int_value);
  }

  return [int_value.toString(), true, ""]
}

// eslint-disable-next-line
function validate_char(value, definition, uid, args) {
  if (value.length != 1) {
    return [0, false, "String being validated can have only one character"];
  }
  return [value.charCodeAt(0), true, ""];
}

// eslint-disable-next-line
function validate_float(value, definition, uid, args) {
  const float_value = parseFloat(value);
  if (isNaN(float_value))
    return [0, false, "Couldn't parse float from: " + float_value];
  return [float_value, true, ""];
}

// eslint-disable-next-line
function display_int(value, definition, uid, args) {
  return value;
}

// eslint-disable-next-line
function display_char(value, definition, uid, args) {
  return String.fromCharCode(value);
}

// eslint-disable-next-line
function display_float(value, definition, uid, args) {
  return value
}

const validator_map = ref({
  // lookup label: [validate_fn, [bits, sign]]
  uint8_t:  [validate_integer, [8, false]],
  int8_t:   [validate_integer, [8, true]],
  uint16_t: [validate_integer, [16, false]],
  int16_t:  [validate_integer, [16, true]],
  uint32_t: [validate_integer, [32, false]],
  int32_t:  [validate_integer, [32, true]],
  uint64_t: [validate_integer, [64, false]],
  int64_t:  [validate_integer, [64, true]],
  int:      [validate_integer, [4, true]],
  char:     [validate_char, [8, false]],
  float:    [validate_float,   []],
  double:   [validate_float,   []],
});

const display_map = ref({
  uint8_t:  [display_int, [8, false]],
  int8_t:   [display_int, [8, true]],
  uint16_t: [display_int, [16, false]],
  int16_t:  [display_int, [16, true]],
  uint32_t: [display_int, [32, false]],
  int32_t:  [display_int, [32, true]],
  uint64_t: [display_int, [64, false]],
  int64_t:  [display_int, [64, true]],
  int:      [display_int, [4,  true]],
  char:     [display_char, [8, false]],
  float:    [display_float, []],
  double:   [display_float, []],
});

/* reference for the schema data */
const device_schema = ref(undefined);
const schema = ref([]);
const def_uids = ref(0);
const schema_table = ref(new Map());

/* feedback from ipc/tethered operations */
const feedback = ref("");
const busy = ref(false);

window.ipcRenderer.invoke('get_schema').then((result) => {
  feedback.value = "Retrieving schema...";
  console.log("Loading static schema:", result);
  assign_uids(result);
  schema.value = result;
  feedback.value = "Retrieved schema";
},
(err) => {
  feedback.value = "Unable to retrieve schema: " + err;
});

function send_update(path) {
  if (busy.value) {
    feedback.value = "Busy! Wait!!";
    return;
  }
  busy.value = true;
  console.log("Writing schema update:", schema.value);
  feedback.value = "Sending update to device...";
  window.ipcRenderer.invoke('update_schema', JSON.parse(JSON.stringify(schema.value)), path).then((result) => {
     busy.value = false;
     console.log("update result: ", result);
     feedback.value = "Success in updating device!";
     device_schema.value = JSON.parse(JSON.stringify(schema.value));
  },
  (err) => {
    busy.value = false;
    feedback.value = "Failure in updating device: " + err;
  });
}

function send_read(path) {
  if (busy.value) {
    feedback.value = "Busy! Wait!!";
    return;
  }
  busy.value = true;
  console.log("Sending schema read");
  feedback.value = "Reading schema from device...";
  window.ipcRenderer.invoke('read_device_config', JSON.parse(JSON.stringify(schema.value)), path).then((result) => {
    busy.value = false;
     assign_uids(result)
     device_schema.value = JSON.parse(JSON.stringify(result));
     schema.value = result;
     feedback.value = "Success in reading schema";
     console.log("Read schema from device: ", result);
  },
  (err) => {
    busy.value = false;
    feedback.value = "Failure in reading schema: " + err;
  });
}

function locate_definition(uid) {
  return schema_table.value.get(uid)
}

function assign_uids_recurse(definition, start_uid) {
  definition.uid = start_uid++;
  schema_table.value.set(definition.uid, definition);

  if (definition.kind == "struct") {
    for (const child_field of definition.fields) {
      child_field.uid = start_uid++;
      schema_table.value.set(child_field.uid, child_field);
    }
    for (const child_def of definition.children) {
      start_uid = assign_uids_recurse(child_def, start_uid);
    }
  } else if (definition.kind == "list") {
    // list
    if (definition.category == "collection") {
      for (const child_def of definition.items) {
        start_uid = assign_uids_recurse(child_def, start_uid);
      }
    } else {
      for (const child_field of definition.items) {
        child_field.uid = start_uid++;
        schema_table.value.set(child_field.uid, child_field);
      }
    }
  }
  return start_uid;
}

function assign_uids(definition, is_root=true) {
  var uid;
  if (is_root) {
    uid = 0;
    def_uids.value = 0;
  } else {
    uid = def_uids.value;
  }
  def_uids.value = assign_uids_recurse(definition, uid);
}

export default {
  name: 'App',
  setup() {
    const selected_uid = ref(-1);
    const selected_port = ref(-1);

    const ctx = ref({selected_uid: selected_uid,
                     selected_port: selected_port,
                     display_map: display_map});
    const ports = ref([]);

    return {
      ctx,
      selected_uid,
      selected_port,
      ports,
      display_map,
    }
  },
  components: {
    DisplayPort,
    SchemaNode,
  },
  methods: {
    schema_update() {
      if (this.selected_port == -1) {
        feedback.value = "Select a port first to sync the schema";
        return;
      }
      send_update(this.ports.value[this.selected_port].path);
    },

    schema_read() {
      if (this.selected_port == -1) {
        feedback.value = "Select a port first to read the schema";
        return;
      }
      send_read(this.ports.value[this.selected_port].path);
    },

    reset_to_device() {
      if (device_schema.value) {
        const s = JSON.parse(JSON.stringify(device_schema.value));
        assign_uids(s);
        schema.value = s;
        feedback.value = "Reverted schema to device";
      } else {
        feedback.value = "No device schema -- read first!";
      }
    },

    get_ports() {
      console.log("Scanning for ports")
      if (busy.value) {
        feedback.value = "Busy! Wait!!";
        return;
      }
      busy.value = true;
      feedback.value = "Scanning for ports..."
      window.ipcRenderer.invoke('serial_paths').then((result) => {
        busy.value = false;
        this.ports.value = result
        console.log("Port scan:", result);
        feedback.value = "Found ports";
        this.selected_port = 0;
      },
      (err) => {
        busy.value = false;
        feedback.value = "Failed to find ports: " + err;
      });
    },

    selected_port(event) {
      console.log("Selected port:", event.target.value);
      this.selected_port = event.target.value;
    },

    /* --------------------
       Tree handling events
       -------------------- */
    selected(uid) {
      console.log("selected id: " + uid);
      this.selected_uid = uid;
    },

    /* -------------------
       List editing events
       ------------------- */
    list_element_edited(uid, value) {
      console.log("list edited: ", uid, " set to: " + value)
      const definition = locate_definition(uid);
      const [validator_fn, args] = validator_map.value[definition.type]

      const [result, valid, msg] = validator_fn(value, definition, uid, args)
      if (valid) {
        definition.value = result;
      } else {
        definition.value = definition.value;
      }
    },

    list_as_string_edited(uid, value) {
      console.log("list-as-string edited: ", uid, " set to: " + value)
      const definition = locate_definition(uid);

      if (definition.items.length + 1 >= definition.capacity) {
        feedback.value = "List at capacity -- can't add more";
        definition.items = definition.items;
        return;
      }

      const [validator_fn, args] = validator_map.value["char"]

      var all_valid = true;
      const list_value = [];
      for (let c of value) {
        const [result, valid, msg] = validator_fn(c, definition, uid, args)
        list_value.push(result);
        if (!valid) {
          all_valid = false;
        }
      }

      if (all_valid) {
        // +1 includes '/0' character
        definition.items = definition.items.slice(0, list_value.length + 1)

        // fill out the list storage to include any new definitions
        const addc = list_value.length - definition.items.length + 1;
        for (let i = 0; i < addc; i++) {
          definition.items.push(JSON.parse(JSON.stringify(definition.type)));
        }

        for (let i = 0; i < list_value.length; i++) {
          definition.items[i].value = list_value[i];
        }
        // '/0' for string termination in c-strings
        definition.items[list_value.length].value = 0;
      } else {
        // todo: handle invalid
        definition.items = definition.items;
      }
    },

    list_element_added(uid) {
      console.log("list added: ", uid);

      const definition = locate_definition(uid);
      if (definition.items.length < definition.capacity) {
        const type_copy = JSON.parse(JSON.stringify(definition.type));
        definition.items.push(type_copy);
        // handle uid assignments of new schema
        assign_uids(type_copy, false);
      } else {
        // at capacity -- warn and prevent
        feedback.value = "List at capacity -- can't add more";
        definition.items = definition.items
      }
    },

    list_element_up(uid, index) {
      console.log("list element moved up")
      if (index == 0) {
        return;
      }
      const definition = locate_definition(uid);
      const move = definition.items[index];
      definition.items[index] = definition.items[index - 1];
      definition.items[index - 1] = move;
    },
    list_element_down(uid, index) {
      console.log("list element moved down")
      const definition = locate_definition(uid);
      if (index + 1 == definition.items.length) {
        return;
      }

      const move = definition.items[index];
      definition.items[index] = definition.items[index + 1];
      definition.items[index + 1] = move;
    },
    list_element_delete(uid, index) {
      console.log("list element delete")
      const definition = locate_definition(uid);
      definition.items = definition.items.slice(0, index).concat(definition.items.slice(index + 1));
    },

    /* ---------------------
       Struct editing events
       --------------------- */
    struct_field_edited(uid, value) {
      console.log("struct field: ", uid, " set to: " + value)
      const definition = locate_definition(uid);
      const [validator_fn, args] = validator_map.value[definition.type]
      const [result, valid, msg] = validator_fn(value, definition, uid, args)
      if (valid) {
        definition.value = result;
      } else {
        definition.value = definition.value;
      }
      console.log("post-assignment:", definition);
    },
  },
  computed: {
    schema() {
      return schema.value;
    },
    ports() {
      return this.ports.value;
    },
    console_feedback() {
      return feedback.value;
    },
  }
}
/*
// struct definition
{
  kind: "struct",
  label: <label>,
  fields: [<primitive typedef>, ...]
  children: [<collection typedef>]
}

// list definition
{
  kind: "list",
  label: <label>,
  type: {<primitive typedef> | <collection typedef>},
  items: [{<primitive typedef> | <collection typedef>}, ...],
  capacity: <maxlength>,
}

// primitive definition
{
  kind: "primitive",
  [name: <name>], (only valid for structs)
  category: "primitive",
  type: <primitive type-label>,
  value: <value>
}
*/
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;

  display: flex;
  flex-direction: row-reverse;

  align-items: baseline;
  align-content: baseline;
}

.hierarchyNavigator {
  position: absolute;
  top: 0;
  left: 0;

  height: 100%;
  width: 260px;
  padding-top: 20px;
  padding-left: 20px;
  padding-right: 20px;

  outline: solid;
  outline-color: #4baecb;
}

.mainPanel {
  position: absolute;
  top: 0;
  left: 300px;
  height: 100%;
  width: calc(100% - 300px);

  text-align: left;
  justify-content: left;
  justify-self: left;
}

.topStrip {
  display: flex;
  padding-top: 20px;
}

.portScan {
  display: flex;
  justify-content: center;
  padding-left: 20px;
}

.portScanButton {
  position: relative;
}

.portSelectionDropdown {
  position: relative;
  top: -2px;
  padding: 20px;
  padding-right: 40px;
  width: 200px;
}

.filler {
  width: 100%;
}

.buttons {
  display: flex;
  justify-content: right;
  width: 100%;
  justify-items: right;
}

.button {
  border: none;
  border-radius: 10pt;
  outline: 0;

  margin-left: 10px;
  margin-right: 10px;

  background-color: #b8b8b8;
  text-align: center;
  vertical-align: middle;
}

.button:active {
  background-color: #4baecb;
}

.resetButton {
}

.readButton {
}

.updateButton {
  margin-right: 10px;
}

.editPanel {
  padding: 20px;


  #display: flex;
  #justify-content: left;
  #flex-direction: row;
  #flex-wrap: wrap;
}

.feedbackStrip {
  position: absolute;
  bottom: 0;

  padding-left: 20px;

  width: 100%;

  text-align: left;
}

.structFieldBox {
  padding-left: 20px;
  padding-right: 20px;
}

.structNameBox {
  padding: 10px;
}

.structInputBox {
  padding: 5px;
}
</style>
