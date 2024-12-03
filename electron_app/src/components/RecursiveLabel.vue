<template>
  <div class="recursivelabel" :style="css_props">
    <div class="hierarchyEntry">
      <p @click="toggle_display_children" :class="arrow_classes"> {{ arrow }} </p>
      <p @click="select()" :class="selected_classes"> {{ navigation_label }} </p>
    </div>
    <!-- struct handling -->
    <div v-if="node.kind == 'struct'">
      <div v-if="ctx.selected_uid == node.uid" class="panelSet">
        <DisplayStruct :definition="node"
                       :ctx="ctx"
                       @struct_field_edited="struct_field_edited"/>
      </div>
      <div v-show="display_children" class="conditionalStuff">
        <RecursiveLabel v-for="(childnode, index) in node.children"
                        :key="index"
                        :node="childnode"
                        :level="level+1"
                        :ctx="ctx"
                        @selected_node="select_node"
                        @list_element_edited="list_element_edited"
                        @list_element_up="list_element_up"
                        @list_element_down="list_element_down"
                        @list_element_delete="list_element_delete"
                        @list_as_string_edited="list_as_string_edited"
                        @list_element_added="list_element_added"
                        @struct_field_edited="struct_field_edited"/>
      </div>
    </div>
    <!-- list handling -->
    <div v-if="node.kind == 'list'">
      <div v-if="ctx.selected_uid == node.uid" class="panelSet">
        <DisplayList :definition="node"
                     :ctx="ctx"
                     @list_element_edited="list_element_edited"
                     @list_element_up="list_element_up"
                     @list_element_down="list_element_down"
                     @list_element_delete="list_element_delete"
                     @list_as_string_edited="list_as_string_edited"
                     @list_element_added="list_element_added"
                     @struct_field_edited="struct_field_edited"/>
      </div>
      <!-- list type is a collection, so we have sub-entries we can list in the tree -->
      <div v-if="node.type.kind != 'primitive'" v-show="display_children" class="conditionalStuff">
        <RecursiveLabel v-for="(item, index) in node.items"
                        :key="index"
                        :node="item"
                        :label="String(index)"
                        :level="level+1"
                        :ctx="ctx"
                        @selected_node="select_node"
                        @list_element_edited="list_element_edited"
                        @list_element_up="list_element_up"
                        @list_element_down="list_element_down"
                        @list_element_delete="list_element_delete"
                        @list_as_string_edited="list_as_string_edited"
                        @list_element_added="list_element_added"
                        @struct_field_edited="struct_field_edited"/>
      </div>
    </div>
  </div>
</template>

<script>
import DisplayStruct from "./DisplayStruct.vue"
import DisplayList from './DisplayList.vue'

export default {
  name: 'RecursiveLabel',
  components: {
    DisplayStruct,
    DisplayList,
  },
  props: {
    node: Object,
    label: String,
    level: Number,
    ctx: Object,
  },
  data() {
    return {
      display_children: true,
    }
  },
  computed: {
    css_props() {
      return {
        '--recursive-padding': (this.level * 10) + "px",
      }
    },
    has_children() {
      if (this.node.kind == "list") {
        return this.node.items.length > 0 && this.node.items[0].kind != "primitive"
      } else if (this.node.kind == "struct") {
        return this.node.children.length > 0
      }
      return false;
    },
    arrow() {
      if (this.has_children) {
        return "v";
      }
      return " ";
    },
    arrow_classes() {
      if (this.display_children) {
        return "hierarchyArrow"
      } else {
        return "hierarchyArrow hierarchyArrowSelected"
      }
    },
    navigation_label() {
      return this.label || this.node.label;
    },
    selected() {
      return this.ctx.selected_uid == this.node.uid;
    },
    selected_classes() {
      if (this.selected) {
        return "hierarchyLabel hierarchyLabelSelected"
      } else {
        return "hierarchyLabel"
      }
    }
  },
  methods: {
    /* ---------------------
       Events from this node
       --------------------- */
    select() {
      this.$emit("selected_node", this.node.uid);
    },
    toggle_display_children() {
      this.display_children = !this.display_children
    },

    /* --------------------
       Tree handling events
       -------------------- */
    select_node(selected_uid) {
      this.$emit("selected_node", selected_uid);
    },

    /* -------------------
       List editing events
       ------------------- */
    list_element_edited(uid, value) {
      this.$emit("list_element_edited", uid, value);
    },
    list_as_string_edited(uid, value) {
      this.$emit("list_as_string_edited", uid, value);
    },
    list_element_added(uid) {
      this.$emit("list_element_added", uid);
    },
    list_element_up(uid, index) {
      this.$emit("list_element_up", uid, index);
    },
    list_element_down(uid, index) {
      this.$emit("list_element_down", uid, index);
    },
    list_element_delete(uid, index) {
      this.$emit("list_element_delete", uid, index);
    },

    /* ---------------------
       Struct editing events
       --------------------- */
    struct_field_edited(uid, value) {
      this.$emit("struct_field_edited", uid, value);
    },
  },
  emits: ['selected_node',
          'list_element_edited',
          'list_as_string_edited',
          'list_element_added',
          'list_element_up',
          'list_element_down',
          'list_element_delete',
          'struct_field_edited']
}
</script>
<style>
.hierarchyEntry {
  display: flex;
}

.hierarchyLabel {
  padding-left: 10px;
  padding-right: 10px;
}

.hierarchyLabelSelected {
  background-color: #4baecb;
}

.hierarchyArrow {
  margin-left: var(--recursive-padding);
  text-align: center;
  width: 20px;
}

.hierarchyArrowSelected {
  transform: rotate(-0.25turn);
}

.the-arrow.-left {
  transform: rotate(180deg);
}

.the-arrow.-right {
  transform: rotate(180deg);
}
</style>
