import qbs.File
import qbs.FileInfo

Probe{
    property string searchPath: FileInfo.joinPaths(project.sourceDirectory, "src/libs")
    property stringList libs: []
    configure: {
        found=false;
        //console.info("--------------------------------------------------------------------------");
        //console.info("Probing for OctoMY libs @"+searchPath+":");
        var raw = File.directoryEntries(searchPath, File.Dirs | File.NoDot | File.NoDotDot);
        //console.info("raw: "+JSON.stringify(raw));
        libs = (raw || []).filter(function(dir){
            //console.info("filtering: "+dir);
            return dir.startsWith("lib") && File.exists(FileInfo.joinPaths(searchPath, dir, "lib.qbs"));
        });
        found = libs.length > 0;
        console.info("Probing returned libs="+libs+", found="+found);
    }
}
