<template>
  <div id="app">
    <bois-bois-go></bois-bois-go>
  </div>
</template>

<script>
import BoisBoisGo from "./components/BoisBoisGo";

export default {
  name: "app",
  components: {
    BoisBoisGo
  }
};
</script>

<style>
#app {
  font-family: "Avenir", Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
  margin-top: 60px;
}
</style>
