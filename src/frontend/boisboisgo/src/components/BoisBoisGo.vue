<template>
  <div class="container">
    <div class="search">
      <img src="./bbg-logo.png" alt="Bois Bois Go Logo">
      <form v-on:submit="search">
        <input type="text" name="Search" v-model="query" />
        <button>Search</button>
      </form>
    </div>

    <search-results v-bind:results="searchResults"></search-results>
  </div>
</template>

<script>
import axios from 'axios';
import SearchResults from './searchResults';

export default {
  name: 'BoisBoisGo',
  components: {
    SearchResults,
  },
  data: function() {
    return {
      query: '',
      searchResults: [],
    };
  },
  methods: {
    search: function(event) {
      event.preventDefault();

      const query = this.query.trim().replace(/\s\s+/g, ' ');
      console.log(query);

      axios.post('/query', query)
        .then((response) => {
          this.searchResults = response.data;
        })
        .catch((error) => {
          console.log(error);
        });
      
    }
  }
};
</script>

<style scoped>
.container {
  margin-left: 20px;
  display: flex;
  flex-direction: column;
}

.search {
  display: flex;
  flex-direction: row;
  justify-items: left;
  align-items: center;
}

img {
  width: 100px;
  height: 100px;
  margin-bottom: 10px;
}

form {
  margin-bottom: 20px;
  margin-top: 20px;
  margin-left: 10px;
}

button {
  margin-left: 10px;
}
</style>