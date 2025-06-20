-- Create the database (if needed)
drop database RayNormalAnalysis;

CREATE DATABASE RayNormalAnalysis;
GO

USE RayNormalAnalysis;
GO

-- Create the table to store ray normal data
CREATE TABLE RayNormals (
    Id INT IDENTITY(1,1) PRIMARY KEY,
    NormalX FLOAT NOT NULL,
    NormalY FLOAT NOT NULL,
    Order1 INT NOT NULL,
    Order2 INT NOT NULL,
    Order3 INT NOT NULL,
    Order4 INT NOT NULL,
    Order5 INT NOT NULL,
    Order6 INT NOT NULL,
    Order7 INT NOT NULL,
    Order8 INT NOT NULL,
    Order9 INT NOT NULL,
    PermutationHash AS (
        CAST(Order1 AS VARCHAR) + ',' +
        CAST(Order2 AS VARCHAR) + ',' +
        CAST(Order3 AS VARCHAR) + ',' +
        CAST(Order4 AS VARCHAR) + ',' +
        CAST(Order5 AS VARCHAR) + ',' +
        CAST(Order6 AS VARCHAR) + ',' +
        CAST(Order7 AS VARCHAR) + ',' +
        CAST(Order8 AS VARCHAR) + ',' +
        CAST(Order9 AS VARCHAR)
    ) PERSISTED
);




-- Create an index on the permutation hash for faster grouping
CREATE INDEX IX_RayNormals_PermutationHash ON RayNormals(PermutationHash);

-- Query to get min/max normal values for each permutation
SELECT 
    PermutationHash,
    Order1, Order2, Order3, Order4, Order5, Order6, Order7, Order8, Order9,
    MIN(NormalX) AS MinNormalX,
    MAX(NormalX) AS MaxNormalX,
    MIN(NormalY) AS MinNormalY,
    MAX(NormalY) AS MaxNormalY,
    COUNT(*) AS RecordCount
FROM 
    RayNormals
GROUP BY 
    PermutationHash,
    Order1, Order2, Order3, Order4, Order5, Order6, Order7, Order8, Order9
ORDER BY 
    PermutationHash;


