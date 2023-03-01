function listProperty(item)
{
    for (var p in item){
        console.log(p + ": " + item[p]);
    }
    return JSON.stringify(item, null, 3);
}

Number.prototype.padLeft = function(base,chr){
    var  len = (String(base || 10).length - String(this).length)+1;
    return len > 0? new Array(len).join(chr || '0')+this : this;
}

function now(){
    var d = new Date;
    return [d.getFullYear().padLeft(), (d.getMonth()+1).padLeft(), d.getDate().padLeft()].join('-')+' '+
            [d.getHours().padLeft(),
             d.getMinutes().padLeft(),
             d.getSeconds().padLeft()].join(':');
}

function printDebug(path)
{
    console.info(path+" Debug@" + now());
}


function sortUnique(list){
    list=list || [];
    list = list.filter(function(value, index, array) {
        return array.indexOf(value) === index;
    });
    list=list.sort();
    return list;
}
