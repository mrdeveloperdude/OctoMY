import qbs.File
import qbs.FileInfo

Probe{
    // Parameters
    property string searchPath: FileInfo.joinPaths(project.sourceDirectory, "src/libs")
    property string libFileName: "lib.qbs"
    property string libDefinePrefix: "OC_USE_"
    // Outputs
    property stringList libNames: []
    property stringList libFolders: []
    property stringList libFiles: []
    property stringList libDefines: []
    configure: {
        found=false;
        //console.info("--------------------------------------------------------------------------");
        //console.info("Probing for OctoMY libs @"+searchPath+":");
        var raw = File.directoryEntries(searchPath, File.Dirs | File.NoDot | File.NoDotDot);
        //console.info("raw: "+JSON.stringify(raw));
        libNames = (raw || []).filter(function(dir){
            //console.info("filtering: "+dir);
            if(libFileName){
                return dir.startsWith("lib") && File.exists(FileInfo.joinPaths(searchPath, dir, libFileName));
            }
            else{
                return dir.startsWith("lib");
            }
        });
        libFolders = libNames.map(function(libName){
            return FileInfo.joinPaths(searchPath, libName);
        })
        libFiles = libFolders.map(function(libFolder){
            return FileInfo.joinPaths(libFolder, libFileName);
        })
        libDefines = libNames.map(function(libName){
            return libDefinePrefix + libName.toUpperCase();
        })
        found = libNames.length > 0;
        console.info("Probing returned;");
        console.info(" + found="+found);
        console.info(" + searchPath="+JSON.stringify(searchPath, null, "\t"));
        console.info(" + libNames="+JSON.stringify(libNames, null, "\t"));
        console.info(" + libFolders="+JSON.stringify(libFolders, null, "\t"));
        console.info(" + libFiles="+JSON.stringify(libFiles, null, "\t"));
    }
}
