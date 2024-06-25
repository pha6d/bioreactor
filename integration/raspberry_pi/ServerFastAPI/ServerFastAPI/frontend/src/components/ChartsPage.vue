<template>
  <div>
    <h1>Charts</h1>
    <div id="chart"></div>
  </div>
</template>

<script>
import Plotly from 'plotly.js-dist';

export default {
  name: 'ChartsPage',
  mounted() {
    this.fetchData();
  },
  methods: {
    async fetchData() {
      try {
        const response = await fetch('http://192.168.1.25:8000/sensor_data');
        if (!response.ok) {
          throw new Error('Network response was not ok');
        }
        const data = await response.json();
        this.plotData(data);
      } catch (error) {
        console.error('Error fetching data:', error);
      }
    },
    plotData(data) {
      const timestamps = data.map(entry => entry.Backend_Time);
      const waterTemp = data.map(entry => parseFloat(entry.waterTemp));
      const airTemp = data.map(entry => parseFloat(entry.airTemp));
      const ph = data.map(entry => parseFloat(entry.ph));
      const turbidity = data.map(entry => parseFloat(entry.turbidity));
      const oxygen = data.map(entry => parseFloat(entry.oxygen));

      const trace1 = {
        x: timestamps,
        y: waterTemp,
        name: 'Water Temperature',
        type: 'scatter'
      };
      const trace2 = {
        x: timestamps,
        y: airTemp,
        name: 'Air Temperature',
        type: 'scatter'
      };
      const trace3 = {
        x: timestamps,
        y: ph,
        name: 'pH',
        type: 'scatter'
      };
      const trace4 = {
        x: timestamps,
        y: turbidity,
        name: 'Turbidity',
        type: 'scatter'
      };
      const trace5 = {
        x: timestamps,
        y: oxygen,
        name: 'Oxygen',
        type: 'scatter'
      };

      const dataToPlot = [trace1, trace2, trace3, trace4, trace5];

      const layout = {
        title: 'Sensor Data',
        xaxis: {
          title: 'Time'
        },
        yaxis: {
          title: 'Values'
        }
      };

      Plotly.newPlot('chart', dataToPlot, layout);
    }
  }
};
</script>

<style scoped>
/* Add any required styles here */
</style>
