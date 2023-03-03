import qbs.FileInfo

Project{
    name: "Tests"
    
    // Enumerate OctoMY test projects
    TestProbe {
        id: octomyTests
    }
    
    //references: octomyTests.files
    references: ["testMarkdownEditor/test.qbs", "testPlanEditor/test.qbs"]
    
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
