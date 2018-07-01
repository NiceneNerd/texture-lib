if(process.env.DEBUG){
    module.exports = require('./build/Debug/texture.node');
}else{
    module.exports = require('./build/Release/texture.node');
}
