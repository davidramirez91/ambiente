import express, { json } from "express";
import cors from "cors";
import mysql from "mysql2/promise";
import pg from "pg";
import { config } from "dotenv";

config(); // dotemv variables de enterno
const { Pool } = pg; // pool de pg
const app = express();
const PORT = process.env.PORT ?? 3000;

app.use(cors()); // oring cabecera
app.use(json()); // req.body
app.set("view engine", "ejs"); // Configura Express para usar EJS
app.use(express.static("views"));
app.disable("x-powered-by");

// Configuración de la conexión a la base de datos PG

const pool = new Pool({
   connectionString: process.env.DATABASE_URL,
   ssl: {
      rejectUnauthorized: false, // Esto es necesario si el servidor utiliza un certificado autofirmado
   },
});

// LLamadas
app.post("/datos", async (req, res) => {
   try {
      // Destructure the data from the request body
      const { humedad_a, temperatura, humedad_s, intensidad } = req.body;

      // Insert data into the database
      const query =
         "INSERT INTO toma_datos(humedad_a, temperatura, humedad_s, intensidad) VALUES ($1, $2, $3, $4)";
      const results = await pool.query(query, [
         humedad_a,
         temperatura,
         humedad_s,
         intensidad,
      ]);
      res.status(201).send({ message: "Datos insertados con éxito", results });
   } catch (error) {
      res.status(500).send({ message: "Error al insertar datos", error });
   }
});
app.get("/datos", async (req, res) => {
   try {
      // Retrieve data from the database
      const query = "SELECT * FROM toma_datos";
      const { rows } = await pool.query(query);

      res.status(200).send(rows);
   } catch (error) {
      res.status(500).send({ message: "Error al obtener datos", error });
   }
});

app.use("/", (req, res) => {
   res.render("index", { title: "EINSPHI" });
});

// ESUCHAR
app.listen(PORT, () => {
   console.log(`puerto: http://localhost:${PORT}`);
});
