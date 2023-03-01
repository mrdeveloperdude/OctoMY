import qbs.File
import qbs.FileInfo

Probe{
    // Parameters
    property string name: "BaseProbe"
    property string searchDir:undefined
    property string fileName:undefined
    property string folderPrefix:undefined
    property string definePrefix:"UNSET_"
    property bool debug: false
    // Outputs
    property string searchPath: FileInfo.joinPaths(project.sourceDirectory, searchDir)
    property stringList names: []
    property stringList folders: []
    property stringList files: []
    property stringList defines: []
    configure: {
        found = false;

        var raw = File.directoryEntries(searchPath, File.Dirs | File.NoDot | File.NoDotDot);
        names = (raw || []).filter(function(dir){
            var ok = true;
            if(debug){
                console.info("Checking "+dir+":");
            }
            if(folderPrefix !== undefined){
                ok = ok && dir.startsWith(folderPrefix);
                if(debug){
                    console.info("Checking folderprefix:"+ok);
                }
            }
            if(fileName  !== undefined){
                var fullPath = FileInfo.joinPaths(searchPath, dir, fileName);
                ok = ok && File.exists(fullPath);
                if(debug){
                    console.info("Checking fullPath:"+fullPath+" gave:"+ok);
                }
            }
            return ok;
        });
        folders = names.map(function(name){
            return FileInfo.joinPaths(searchPath, name);
        })
        files = folders.map(function(folder){
            return FileInfo.joinPaths(folder, fileName);
        })
        defines = names.map(function(name){
            return definePrefix + name.toUpperCase();
        })
        found = names.length > 0;
        if(debug){
            console.info("Files for '" + name + "':");
            console.info("    searchDir: " + searchDir);
            console.info("     fileName: " + fileName);
            console.info(" folderPrefix: " + folderPrefix);
            console.info(" definePrefix: " + definePrefix);
            console.info("        found: " + found);
            console.info("          raw: " + JSON.stringify(raw, null, "\t"));
            console.info("   searchPath: " + JSON.stringify(searchPath, null, "\t"));
            console.info("        names: " + JSON.stringify(names, null, "\t"));
            console.info("      folders: " + JSON.stringify(folders, null, "\t"));
            console.info("        files: " + JSON.stringify(files, null, "\t"));
            console.info("      defines: " + JSON.stringify(defines, null, "\t"));
        }
    }
}
