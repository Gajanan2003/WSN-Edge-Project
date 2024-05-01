const mongoose = require("mongoose");

// Define the schema
const schema = new mongoose.Schema({
    CarName: {
        type: String,
        required: true
    },
    StrUID: {
        type: String,
        required: true,
    }
}, {
    timestamps: {
        createdAt: 'created_at',
        updatedAt: 'updated_at'
    }
});

// Create the model from the schema
const CarData = mongoose.model("CarData", schema);
module.exports = CarData