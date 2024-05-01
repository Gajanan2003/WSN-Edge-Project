// server.js
const express = require('express');
const path = require('path');
const CarData = require('./struct')
require("./db");

// Create an Express application
const app = express();

// Middleware to parse JSON bodies
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));



app.get('/data', async(req, res) => {
    try {
        const cars = await CarData.find();
        res.json(cars);
    } catch (error) {
        res.status(500).send("Error retrieving car data");
    }
});


app.post('/data', async(req, res) => {
    // Extract CarName and StrUID from the request body
    const { CarName, StrUID } = req.body;

    // Check if both CarName and StrUID are provided
    if (!CarName) {
        return res.status(400).json({ message: "Car name is required" });
    }
    if (!StrUID) {
        return res.status(400).json({ message: "StrUID is required" });
    }

    try {
        // Create a new car data entry using the CarData model
        const newCarData = new CarData({
            CarName,
            StrUID // Include StrUID in the document to be saved
        });

        // Save the new car data to the database
        const savedCarData = await newCarData.save();

        // Respond with the saved car data
        res.status(201).json(savedCarData);
    } catch (error) {
        // Handle potential errors, such as unique constraint violations
        if (error.code === 11000) {
            return res.status(409).json({ message: "Duplicate StrUID, it must be unique" });
        }
        console.error("Error saving car data:", error);
        res.status(500).json({ message: "Error saving car data" });
    }
});


app.post('/exit', async(req, res) => {
    // Get CarName from the request body
    const CarName = req.body.CarName;

    // Check if CarName is provided
    if (!CarName) {
        return res.status(400).json({ message: "Car name is required" });
    }

    try {
        // Create a new car data entry using the CarData model
        const newCarData = new CarData({
            CarName
        });

        // Save the new car data to the database
        const savedCarData = await newCarData.save();

        // Respond with the saved car data
        res.status(201).json(savedCarData);
    } catch (error) {
        // Handle potential errors
        console.error("Error saving car data:", error);
        res.status(500).json({ message: "Error saving car data" });
    }
});


// Start the server
const PORT = process.env.PORT || 5000;
app.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
});