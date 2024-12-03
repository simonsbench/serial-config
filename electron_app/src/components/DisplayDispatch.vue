<template>
  <div v-if="definition.kind == 'struct'">
    <!-- end of the line -->
    <DisplayStruct :key="definition.id"
                   :definition="definition"
                   :ctx="ctx"
                   @struct_field_edited="struct_field_edited"/>
  </div>
  <div v-else>
    <!-- there could be sub-elements added... should make both events available -->
    <DisplayList :key="definition.id"
                 :definition="definition"
                 :ctx="ctx"
                 @list_element_edited="list_element_edited"
                 @list_element_added="list_element_added"
                 @list_as_string_edited="list_as_string_edited"
                 @struct_field_edited="struct_field_edited"/>
  </div>
</template>

<script>
import { defineAsyncComponent, computed } from 'vue'

export default {
  name: 'DisplayDispatch',
  components: {
  },
  beforeCreate() {
    this.$options.components.DisplayList = defineAsyncComponent(() => import('./DisplayList.vue'));
    this.$options.components.DisplayStruct = defineAsyncComponent(() => import('./DisplayStruct.vue'));
  },
  props: {
    definition: Object,
    ctx: Object,
    index: Number, // index in list above
  },
  methods: {
    /* ---------------------
       Struct editing events
       --------------------- */
    struct_field_edited(uid, value) {
      this.$emit("substruct_element_edited", uid, value);
    },
    list_element_edited(uid, value) {
      this.$emit("sublist_element_edited", uid, value);
    },
    list_element_added(uid) {
      this.$emit("sublist_element_added", uid);
    },
  }
}
</script>
