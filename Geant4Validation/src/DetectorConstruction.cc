#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // --- Define materials ---
    // geant4 material manager 
    auto nist = G4NistManager::Instance();

    auto vacuum = nist->FindOrBuildMaterial("G4_Galactic");
    auto water = nist->FindOrBuildMaterial("G4_WATER");

    // World: 2 m x 2 m x 2 m - g4box takes half-lenghts
    auto solidWorld = new G4Box("World", 1.0*m, 1.0*m, 1.0*m);
    // create logical volume by combining geometric solid, material, name
    auto logicWorld = new G4LogicalVolume(solidWorld, vacuum, "World");

    auto physWorld = new G4PVPlacement(
        nullptr, // no rotation
        G4ThreeVector(), // at (0,0,0)
        logicWorld, // logical volume being placed
        "World",
        nullptr, // no parent (this is the volume)
        false,
        0,
        true // check for overlaps
    );

    // Water phantom envelope: 8 cm x 6 cm x 4 mm
    auto solidWater = new G4Box(
        "Water",
        4.0*cm,
        3.0*cm,
        0.2*cm
    );

    auto logicWater = new G4LogicalVolume(
        solidWater,
        water,
        "Water"
    );

    // Place phantom with its entrance face at z = 0
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(0., 0., 0.2*cm),
        logicWater,
        "Water",
        logicWorld,
        false,
        0,
        true
    );

    // X division: 1 replica
    auto solidX = new G4Box(
        "XDivision",
        4.0*cm,
        3.0*cm,
        0.2*cm
    );

    auto logicX = new G4LogicalVolume(
        solidX,
        water,
        "XDivision"
    );

    new G4PVReplica(
        "XDivision",
        logicX,
        logicWater,
        kXAxis,
        1,
        8.0*cm
    );

    // Y division: 1 replica
    auto solidY = new G4Box(
        "YDivision",
        4.0*cm,
        3.0*cm,
        0.2*cm
    );

    auto logicY = new G4LogicalVolume(
        solidY,
        water,
        "YDivision"
    );

    new G4PVReplica(
        "YDivision",
        logicY,
        logicX,
        kYAxis,
        1,
        6.0*cm
    );

    // Z division: 400 replicas, each 0.01 mm thick
    auto solidZ = new G4Box(
        "ZDivision",
        4.0*cm,
        3.0*cm,
        0.005*mm
    );

    auto logicZ = new G4LogicalVolume(
        solidZ,
        water,
        "ZDivision"
    );

    new G4PVReplica(
        "ZDivision",
        logicZ,
        logicY,
        kZAxis,
        400,
        0.01*mm
    );

    // Visualisation
    // make world invisible in visualisation
    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

    auto waterVis = new G4VisAttributes(
        G4Colour(0.2, 0.5, 1.0)
    );

    waterVis->SetVisibility(true);
    logicZ->SetVisAttributes(waterVis);

    return physWorld;
}
