// This is a very small demo that is intended to demonstrate the core functionality
// of the project and not any of the advanced features.

const express = require('express');
const app = express();
const port = 5000;

let inMemoryDataStorage = [];

app.use(express.json());

app.get('/', (req, res) => {
  res.send('Demo Registry Up!');
});

app.post('/test', (req, res) => {
  console.log('Received POST data:', req.body);
  res.status(201).send({ message: 'POST received', data: req.body });
});

app.get('/get-nodes', (req, res) => {
  res.status(200).json(inMemoryDataStorage);
});

app.post('/register-node', (req, res) => {
  try {
    const data = {
      domain: req.body.domain,
      ip: req.body.ip,
      port: req.body.port,
      occupied: req.body.occupied
    };

    const index = inMemoryDataStorage.findIndex(obj => obj.port === data.port);

    if (index !== -1) {
      inMemoryDataStorage[index] = { ...inMemoryDataStorage[index], ...data };
    } else {
      inMemoryDataStorage.push(data);
    }

    res.status(204).send();
  } catch (error) {
    res.status(400).json({ message: error });
  }
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});