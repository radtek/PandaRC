/**
 * 获取字符串的哈希值
 * @param {String} str
 * @param {Boolean} caseSensitive
 * @return {Number} hashCode
 */
function hashCode(str,caseSensitive) {
    if(!caseSensitive){
        str = str.toLowerCase();
    }
    // 1315423911=b'1001110011001111100011010100111'
    var hash = 1315423911,i,ch;
    for (i = str.length - 1; i >= 0; i--) {
        ch = str.charCodeAt(i);
        hash ^= ((hash << 5) + ch + (hash >> 2));
    }
  
    return  (hash & 0x7FFFFFFF);
}




let hash = {};
hash.encode = function(str, caseSensitive) {
    return hashCode(str, caseSensitive);
}
module.exports = hash;