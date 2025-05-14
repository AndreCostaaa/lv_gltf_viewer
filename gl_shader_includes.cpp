#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> /* strlen, strcpy: for replaceWord and lineCount */
 
#include "gl_shader_includes.h"

#include <iostream> /* for debug cout */
#include <sstream>  /* for replaceWord */
#include <fstream>  /* for replaceWord */
#include <vector>   /* for addDefine, clearDefines */
#include <map>
#include <string>

//using namespace std;
bool _vdefines_initialized = false;
std::vector<key_value> v_defines;

void __init_vdefines(void) { 
    if (_vdefines_initialized) return;
    _vdefines_initialized = true;
    v_defines = std::vector<key_value>(); }

key_value* all_defines(void) {
    return &v_defines[0];
}
unsigned int all_defines_count(void) {
    return v_defines.size();
}

void _clearDefines(void){
    __init_vdefines();
    v_defines.erase(v_defines.begin(), v_defines.end());
    v_defines.clear();
    v_defines.shrink_to_fit();
}

void _addDefine( const char* defsymbol, const char* defvalue_or_null ){
    __init_vdefines();
    for (auto &_kv : v_defines){
        if (strcmp(_kv.key, defsymbol) == 0) {
            return; } }
    key_value _newkv = key_value({defsymbol, defvalue_or_null});
    v_defines.push_back(_newkv);
}

char* getDefineId(void) {
    char* _tretstr = (char*)malloc(1024);
    _tretstr[0] = '\0';
    bool _firstpass = true;
    for (auto &_kv : v_defines){
        if (!_firstpass) { 
            strcat(_tretstr, "|"); }
        _firstpass = false;
        strcat(_tretstr, _kv.key); 
        if (_kv.value) { 
            strcat(_tretstr, _kv.value); } }
    return _tretstr; }

std::string replaceWord(std::string s, const char* c_f, const char* c_r ) {
    //std::string s = std::string(c_s);
    std::string f = std::string(c_f);
    std::string r = std::string(c_r);
    
    if ( s.empty() || f.empty() || f == r || s.find(f) == std::string::npos ) {
        return (s);
    }

    std::ostringstream build_it;
    size_t i = 0;
    for ( size_t pos; ( pos = s.find( f, i ) ) != std::string::npos; ) {
        build_it.write( &s[i], pos - i );
        build_it << r;
        i = pos + f.size();
    }
    if ( i != s.size() ) {
        build_it.write( &s[i], s.size() - i );
    }
    return build_it.str();
}

size_t lineCount(const char* str) {
    size_t count = 0;
    while (*str) {
        if (*str == '\n') {
            count++;
        }
        str++;
    }
    return count;
}

char* PREPROCESS(const char* x) {
    char* _def = get_defines_str(); 
    char* _ret = process_includes(x, _def ); 
    free(_def);
    return _ret;
}

char* get_defines_str(void) {
    __init_vdefines();
    std::string _ret = std::string(GLSL_VERSION_PREFIX) + std::string("\n");

    for (auto &_kv : v_defines){
        _ret += "#define " + std::string(_kv.key);
        if (_kv.value != NULL) {
            _ret += " " + std::string(_kv.value);
        }
        _ret += "\n";
    }
    char* _retcstr = (char*)malloc(_ret.length() + 1);
    _retcstr[0] = '\0';
    strcpy(_retcstr, _ret.c_str());
    return _retcstr;
}

char* process_defines( const key_value* __define_set, size_t _num_items){
    uint _reqlength = strlen(GLSL_VERSION_PREFIX) + 1;
    for (size_t i=0; i<_num_items; i++) {
        _reqlength += strlen("#define ");
        _reqlength += strlen(__define_set[i].key);
        if (__define_set[i].value != NULL) {
            _reqlength += strlen(" ");
            _reqlength += strlen(__define_set[i].value);
        }
        _reqlength += strlen("\n");
    }
    char* ret = (char*)malloc(_reqlength + 1);
    ret[0] = '\0';
    strcat(ret, GLSL_VERSION_PREFIX);
    strcat(ret, "\n");
    for (size_t i=0; i<_num_items; i++) {
        strcat(ret, "#define ");
        strcat(ret, __define_set[i].key);
        if (__define_set[i].value != NULL) {
            strcat(ret, " ");
            strcat(ret, __define_set[i].value);
        }
        strcat(ret, "\n");
    }
    //std::cout << "SHADER DEFINES:\n==============\n" << ret << "==============\n";
    return ret;
}

char* process_includes( const char* c_src, const char* _defines ) {
    std::string _src = std::string(c_src);
    std::string  rep = replaceWord(_src, GLSL_VERSION_PREFIX, _defines);
    size_t num_items = sizeof(src_includes) / sizeof(key_value);
    char* _srch = (char*)malloc(255);
    for (size_t i=0; i<num_items; i++) {
        _srch[0] = '\0';
        strcat(_srch, "\n#include <"); 
        strcat(_srch, src_includes[i].key);
        strcat(_srch, ">");
        rep = replaceWord(rep, _srch, src_includes[i].value); }
    free(_srch); 
    char * retval = (char*)malloc(rep.length() + 1);
    retval[0] = '\0';
    strcat(retval, rep.c_str());
    return retval;
}
