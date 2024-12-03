<template>
  <div class="displaylist">
    <Teleport to="#panel">
      <!-- list type is a primitive, non-char: list all the entries -->
      <div v-if="definition.type.kind == 'primitive' && definition.type.type != 'char'" class="listEntries">
        <div v-for="(item, index) of definition.items"
             :key="index"
             class="listEntry">
          <div class="listBox">
            <form>
              <p>
                <label for="name" class="listLabel"> {{ index }}: </label>
                <input :value="element_display(item)"
                       @input="element_edited(item, $event.target.value)"
                       class="listForm"/>
              </p>
            </form>
          </div>
          <div class="listControls">
            <button v-on:click="element_up(index)" class="button upBotton">
              v
            </button>
            <button v-on:click="element_down(index)" class="button downButton">
              v
            </button>
            <button v-on:click="element_delete(index)" class="button deleteButton">
              x
            </button>
          </div>
        </div>
      </div>
      <!-- list type is a primitive, char: collect the entries into a string -->
      <div v-else-if="definition.type.kind == 'primitive' && definition.type.type == 'char'" class="listBox">
          <form>
          <p>
            <label for="name" class="listLabel">{{ definition.label }}</label>
            <input :value="list_as_string"
                   :maxlength="definition.capacity - 1"
                   @input="list_as_string_edited($event.target.value)"/>
          </p>
        </form>
      </div>
      <!-- list type is a collection: splat any entries in sub-fields ="definition.type.kind != 'primitive'" -->
      <div v-else class="list sub-collection">
        <DisplayDispatch v-for="(childnode, index) in definition.items"
                        :key="index"
                        :definition="childnode"
                        :ctx="ctx"
                        :index="index"
                        @sublist_element_edited="sublist_element_edited"
                        @sublist_element_added="sublist_element_added"
                        @substruct_element_edited="substruct_element_edited"/>
      </div>
      <div v-if="!(definition.type.kind == 'primitive' && definition.type.type == 'char')">
        <button v-on:click="element_added()" class="button addButton">
          +
        </button>
      </div>
    </Teleport>
  </div>
</template>

<script>
import DisplayDispatch from "./DisplayDispatch.vue"

export default {
  name: 'DisplayStruct',
  components: {
    DisplayDispatch,
  },
  props: {
    definition: Object,
    ctx: Object,
  },
  computed: {
    list_as_string() {
      console.log("displaying: ", this.definition.items);
      const string_items = []
      for (let item of this.definition.items.slice(0, -1)) {
        const [display_fn, args] = this.ctx.display_map["char"];
        const c = display_fn(item.value, item, item.uid, args);
        console.log("Validated character:", c);
        string_items.push(c);
      }
      return string_items.join("");
    }
  },
  methods: {
    element_display(element_def) {
      const [display_fn, args] = this.ctx.display_map[element_def.type];
      const display_value = display_fn(element_def.value, element_def, element_def.uid, args);
      return display_value;
    },
    // primitive elements events
    element_edited(element_def, value) {
      this.$emit("list_element_edited", element_def.uid, value);
    },
    list_as_string_edited(value) {
      this.$emit("list_as_string_edited", this.definition.uid, value);
    },
    element_added() {
      this.$emit("list_element_added", this.definition.uid);
    },
    element_up(index) {
      this.$emit("list_element_up", this.definition.uid, index);
    },
    element_down(index) {
      this.$emit("list_element_down", this.definition.uid, index);
    },
    element_delete(index) {
      this.$emit("list_element_delete", this.definition.uid, index);
    },

    // collection elements panel
    sublist_element_edited(uid, value) {
      this.$emit("list_element_edited", uid, value);
    },
    sublist_element_added(uid) {
      this.$emit("list_element_added", uid);
    },
    substruct_element_edited(uid, value) {
      this.$emit("struct_field_edited", uid, value);
    },
  }
}
</script>
<style>

.listEntries {

}

.listEntry {
  display: flex;
}

.listBox {
}

.listLabel {
  padding-left: 10px;
  padding-right: 10px;
}

.listForm {
  padding-left: 5px;
  padding-right: 20px;
  margin-right: 20px;
}

.listControls {

}

.upBotton {
  transform: rotate(180deg);
}

.button {
  margin-left: 5px;
  margin-right: 5px;
}
</style>
