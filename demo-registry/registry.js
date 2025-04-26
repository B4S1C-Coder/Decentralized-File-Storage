const express = require('express');
const app = express();
const port = 5000;

app.use(express.json());

app.get('/', (req, res) => {
  res.send('Demo Registry Up!');
});

app.post('/test', (req, res) => {
  console.log('Received POST data:', req.body);
  res.status(201).send({ message: 'POST received', data: req.body });
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});