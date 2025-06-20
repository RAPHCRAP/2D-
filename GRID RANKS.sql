-- Create the database (if needed)
DROP DATABASE RayNormalAnalysis;

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


	-- Query to get min/max normal values with theta angles for each permutation
WITH PermutationStats AS (
    SELECT 
        CONCAT(Order1, ',', Order2, ',', Order3, ',', Order4, ',', Order5, ',', 
               Order6, ',', Order7, ',', Order8, ',', Order9) AS Permutation,
        Order1, Order2, Order3, Order4, Order5, Order6, Order7, Order8, Order9,
        MIN(NormalX) AS MinNormalX,
        MAX(NormalX) AS MaxNormalX,
        MIN(NormalY) AS MinNormalY,
        MAX(NormalY) AS MaxNormalY,
        COUNT(*) AS RecordCount
    FROM 
        RayNormals
    GROUP BY 
        Order1, Order2, Order3, Order4, Order5, Order6, Order7, Order8, Order9
)
SELECT 
    Permutation,
    Order1, Order2, Order3, Order4, Order5, Order6, Order7, Order8, Order9,
    MinNormalX,
    MaxNormalX,
    MinNormalY,
    MaxNormalY,
    -- Calculate theta angles in degrees (0° to 360°)
    DEGREES(ATN2(MinNormalY, MinNormalX)) AS MinThetaDegrees,
    DEGREES(ATN2(MaxNormalY, MaxNormalX)) AS MaxThetaDegrees,
    RecordCount
FROM 
    PermutationStats
ORDER BY 
    -- Sort by average theta angle
    (DEGREES(ATN2(MinNormalY, MinNormalX)) + DEGREES(ATN2(MaxNormalY, MaxNormalX))) / 2;






	-- Query to show permutation angles in 0-360° range
WITH PermutationStats AS (
    SELECT 
        CONCAT(Order1, ',', Order2, ',', Order3, ',', Order4, ',', Order5, ',', 
               Order6, ',', Order7, ',', Order8, ',', Order9) AS Permutation,
        Order1, Order2, Order3, Order4, Order5, Order6, Order7, Order8, Order9,
        MIN(NormalX) AS MinNormalX,
        MAX(NormalX) AS MaxNormalX,
        MIN(NormalY) AS MinNormalY,
        MAX(NormalY) AS MaxNormalY,
        COUNT(*) AS RecordCount
    FROM 
        RayNormals
    GROUP BY 
        Order1, Order2, Order3, Order4, Order5, Order6, Order7, Order8, Order9
),
AngleCalculations AS (
    SELECT 
        Permutation,
        Order1, Order2, Order3, Order4, Order5, Order6, Order7, Order8, Order9,
        RecordCount,
        -- Convert angles to 0-360° range (adding 360 to negative values)
        CASE 
            WHEN DEGREES(ATN2(MinNormalY, MinNormalX)) < 0 
            THEN DEGREES(ATN2(MinNormalY, MinNormalX)) + 360
            ELSE DEGREES(ATN2(MinNormalY, MinNormalX))
        END AS MinThetaDegrees,
        CASE 
            WHEN DEGREES(ATN2(MaxNormalY, MaxNormalX)) < 0 
            THEN DEGREES(ATN2(MaxNormalY, MaxNormalX)) + 360
            ELSE DEGREES(ATN2(MaxNormalY, MaxNormalX))
        END AS MaxThetaDegrees
    FROM 
        PermutationStats
)
SELECT 
    Permutation,
    Order1, Order2, Order3, Order4, Order5, Order6, Order7, Order8, Order9,
    MinThetaDegrees,
    MaxThetaDegrees,
    RecordCount
FROM 
    AngleCalculations
ORDER BY 
    -- Sort by average angle (0-360°)
    (MinThetaDegrees + MaxThetaDegrees) / 2;






-- Delete records where any order column has value around -431,602,000
DELETE FROM RayNormals
WHERE 
    Order1 BETWEEN -431700000 AND -431500000 OR
    Order2 BETWEEN -431700000 AND -431500000 OR
    Order3 BETWEEN -431700000 AND -431500000 OR
    Order4 BETWEEN -431700000 AND -431500000 OR
    Order5 BETWEEN -431700000 AND -431500000 OR
    Order6 BETWEEN -431700000 AND -431500000 OR
    Order7 BETWEEN -431700000 AND -431500000 OR
    Order8 BETWEEN -431700000 AND -431500000 OR
    Order9 BETWEEN -431700000 AND -431500000;

SELECT * FROM RayNormals;



-- Create the table to store ray angle data
CREATE TABLE RayAngles (
    Id INT IDENTITY(1,1) PRIMARY KEY,
    AngleDegrees FLOAT NOT NULL,  -- Stored in 0-360 degree range
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
    ) PERSISTED,
    -- Computed columns for Cartesian coordinates if needed
    NormalX AS (COS(RADIANS(AngleDegrees))) PERSISTED,
    NormalY AS (SIN(RADIANS(AngleDegrees))) PERSISTED
);

-- Create an index on the permutation hash for faster grouping
CREATE INDEX IX_RayAngles_PermutationHash ON RayAngles(PermutationHash);

-- Optional index on the angle itself if you'll query by angle ranges
CREATE INDEX IX_RayAngles_AngleDegrees ON RayAngles(AngleDegrees);