#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

DetectorConstruction::DetectorConstruction(SimulationConfig& config)
    : G4VUserDetectorConstruction(),
      fConfig(config),
      fMessenger(this, "/phantom/", "Phantom configuration")
{
    fMessenger.DeclarePropertyWithUnit(
        "sizeX", "mm", fConfig.sizeX, "Phantom X dimension"
    );

    fMessenger.DeclarePropertyWithUnit(
        "sizeY", "mm", fConfig.sizeY, "Phantom Y dimension"
    );

    fMessenger.DeclarePropertyWithUnit(
        "sizeZ", "mm", fConfig.sizeZ, "Phantom Z dimension"
    );

    fMessenger.DeclareProperty(
        "material", fConfig.material, "Phantom material"
    );

    fMessenger.DeclareProperty(
        "nBins", fConfig.nBins, "Number of depth bins"
    );
}


G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // --- Define materials ---
    auto nist = G4NistManager::Instance();

    auto vacuum = nist->FindOrBuildMaterial("G4_Galactic");
    auto material = nist->FindOrBuildMaterial(fConfig.material);

    // Convert full dimensions to half-lengths
    G4double halfX = fConfig.sizeX / 2.0;
    G4double halfY = fConfig.sizeY / 2.0;
    G4double halfZ = fConfig.sizeZ / 2.0;

    // World: 2 m x 2 m x 2 m
    auto solidWorld = new G4Box(
        "World", 1.0*m, 1.0*m, 1.0*m
    );

    auto logicWorld = new G4LogicalVolume(
        solidWorld, vacuum, "World"
    );

    auto physWorld = new G4PVPlacement(
        nullptr,
        G4ThreeVector(),
        logicWorld,
        "World",
        nullptr,
        false,
        0,
        true
    );

    // --- Water phantom ---
    auto solidWater = new G4Box(
        "Water",
        halfX,
        halfY,
        halfZ
    );

    auto logicWater = new G4LogicalVolume(
        solidWater,
        material,
        "Water"
    );

    // Place phantom with entrance face at z = 0
    new G4PVPlacement(
        nullptr,
        G4ThreeVector(0., 0., halfZ),
        logicWater,
        "Water",
        logicWorld,
        false,
        0,
        true
    );

    // --- X division: 1 replica ---
    auto solidX = new G4Box(
        "XDivision",
        halfX,
        halfY,
        halfZ
    );

    auto logicX = new G4LogicalVolume(
        solidX,
        material,
        "XDivision"
    );

    new G4PVReplica(
        "XDivision",
        logicX,
        logicWater,
        kXAxis,
        1,
        fConfig.sizeX
    );

    // --- Y division: 1 replica ---
    auto solidY = new G4Box(
        "YDivision",
        halfX,
        halfY,
        halfZ
    );

    auto logicY = new G4LogicalVolume(
        solidY,
        material,
        "YDivision"
    );

    new G4PVReplica(
        "YDivision",
        logicY,
        logicX,
        kYAxis,
        1,
        fConfig.sizeY
    );

    // --- Z division: 400 replicas ---
    G4double binWidth = fConfig.sizeZ / fConfig.nBins;

    auto solidZ = new G4Box(
        "ZDivision",
        halfX,
        halfY,
        binWidth / 2.0
    );

    auto logicZ = new G4LogicalVolume(
        solidZ,
        material,
        "ZDivision"
    );

    new G4PVReplica(
        "ZDivision",
        logicZ,
        logicY,
        kZAxis,
        fConfig.nBins,
        binWidth
    );

    // --- Visualisation ---
    logicWorld->SetVisAttributes(
        G4VisAttributes::GetInvisible()
    );

    auto waterVis = new G4VisAttributes(
        G4Colour(0.2, 0.5, 1.0)
    );

    waterVis->SetVisibility(true);
    logicZ->SetVisAttributes(waterVis);

    return physWorld;
}