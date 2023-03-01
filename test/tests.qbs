import qbs.FileInfo

Project{
    name: "Tests"
    
    // Enumerate OctoMY test projects
    TestProbe {
        id: octomyTests
    }
    
    references: octomyTests.files
    
    /*
    AutotestRunner { 
        workingDir:{
            console.log("LOG");
            console.info("AutotestRunner: "+limitToSubProject);
            return undefined;
        }
        limitToSubProject:false
    }*/
}
