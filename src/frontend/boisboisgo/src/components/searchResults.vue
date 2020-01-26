<template>
    <div class="search-results">
        <h3 v-if="results.length > 0">Results:</h3>
        <div class="result" v-for="result in results" v-bind:key="result">
            <a v-bind:href="result.url" target="_blank">{{result.title}}</a>
        </div>
    </div>
</template>

<script>
export default {
  name: "SearchResults",
  props: ['results'],
};
</script>

<style>
.search-results {
    display: flex;
    flex-direction: column;
}

.result {
    margin-top: 10px;
}
</style>
