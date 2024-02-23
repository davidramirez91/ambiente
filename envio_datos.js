import pg from "pg";
const { Pool } = pg;
import { config } from "dotenv";
config(); // Carga las variables de entorno

const pool = new Pool({
   connectionString: process.env.DATABASE_URL,
   ssl: {
      rejectUnauthorized: false, // Esto es necesario si el servidor utiliza un certificado autofirmado
   },
});

// Datos de ejemplo para insertar
const datosParaInsertar = [
   {
      humedad_a: 45.1234,
      temperatura: 23.4567,
      humedad_s: 78.9101,
      intensidad: 12.3456,
   },
   {
      humedad_a: 50.6789,
      temperatura: 25.6789,
      humedad_s: 80.1112,
      intensidad: 14.5678,
   },
   // Agrega más datos según necesites
];

// Función para insertar datos
async function insertarDatos(datos) {
   const client = await pool.connect();
   try {
      await client.query("BEGIN");
      const insertQuery = `
            INSERT INTO toma_datos(humedad_a, temperatura, humedad_s, intensidad)
            VALUES ($1, $2, $3, $4)
        `;
      for (let dato of datos) {
         const valores = [
            dato.humedad_a,
            dato.temperatura,
            dato.humedad_s,
            dato.intensidad,
         ];
         await client.query(insertQuery, valores);
      }
      await client.query("COMMIT");
      console.log("Datos insertados exitosamente");
   } catch (e) {
      await client.query("ROLLBACK");
      throw e;
   } finally {
      client.release();
   }
}

// Llamada a la función insertarDatos
insertarDatos(datosParaInsertar).catch(console.error);
