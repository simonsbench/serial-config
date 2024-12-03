<template>
  <div class="displaystruct">
    <Teleport to="#panel">
      <div v-for="(field_def, index) in definition.fields" class="structFieldBox"
           :key="index">
        <form>
          <label for="name" class="structNameBox"> {{ field_def.label }} </label>
          <input :value="field_display(field_def)"
                 @input="field_edited(field_def, $event.target.value)"
                 class="structInputBox"/>
        </form>
      </div>
      <div v-for="(child_def, index) in display_children"
           :key="index">
        <DisplayList :key="index"
                     :definition="child_def"
                     :ctx="ctx"
                     @list_as_string_edited="list_as_string_edited"/>
      </div>
    </Teleport>
  </div>
</template>

<script>
import DisplayList from "./DisplayList.vue"

export default {
  name: 'DisplayStruct',
  components: {
    DisplayList
  },
  props: {
    definition: Object,
    ctx: Object,
  },
  computed: {
    display_children() {
      const results = [];
      for (const child_def of this.definition.children) {
        if (child_def.kind == "list" && child_def.type.kind == "primitive" && child_def.type.type == "char") {
          results.push(child_def)
        }
      }
      return results
    },
  },
  methods: {
    field_edited(field_def, value) {
      this.$emit("struct_field_edited", field_def.uid, value);
    },
    field_display(field_def) {
      const [display_fn, args] = this.ctx.display_map[field_def.type];
      const display_value = display_fn(field_def.value, field_def, field_def.uid, args);
      return display_value;
    },
    list_as_string_edited(uid, value) {
      this.$emit("list_as_string_edited", uid, value);
    },
  },
}
</script>
