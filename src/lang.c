#include <stdlib.h>
#include <string.h>

#include "lang.h"
#include "util.h"


static int added_langs_ctr = 0;

#define DEF_LANG_SPEC {1,0, 0, 0, CASE_SENS,CASE_SENS,CASE_SENS}


lang_spec_t langs_tmpl[] = {
    { "asm",     "asm|s|S",                         "",                  "", "", DEF_LANG_SPEC },
    { "cc",      "",                                "",                  "", "(?<!\\.mod)(\\.c)$", {0, 0, 0, 1, CASE_SENS,  CASE_SENS,  CASE_SENS} },
    { "hh",      "",                                "",                  "", "(?<!\\.mod)(\\.h)$", {0, 0, 0, 1, CASE_SENS,  CASE_SENS,  CASE_SENS}},
    { "ch",      "",                                "",                  "", "(?<!\\.mod)(\\.(c|h))$", {0, 0, 0, 1, CASE_SENS,  CASE_SENS,  CASE_SENS}},
    { "chs",     "",                                "",                  "", "(?<!\\.mod)(\\.(c|h|s|S))$", {0, 0, 0, 1, CASE_SENS,  CASE_SENS,  CASE_SENS} },
    { "cpp",     "cpp|cc|C|cxx|m|hpp|hh|h|H|hxx",   "",                  "", "", DEF_LANG_SPEC },
    { "css",     "css",                             "",                  "", "", DEF_LANG_SPEC },
    { "html",    "htm|html|shtml|xhtml",            "",                  "", "", DEF_LANG_SPEC },
    { "java",    "java|properties",                 "",                  "", "", DEF_LANG_SPEC },
    { "json",    "json",                            "",                  "", "", DEF_LANG_SPEC },
    { "less",    "less",                            "",                  "", "", DEF_LANG_SPEC },
    { "lisp",    "lisp|lsp",                        "",                  "", "", DEF_LANG_SPEC },
    { "m4",      "m4",                              "",                  "", "", DEF_LANG_SPEC },
    { "perl",    "pl|pm|pm6|pod|t",                 "",                  "", "", DEF_LANG_SPEC },
    { "php",     "php|phpt|php3|php4|php5|phtml",   "",                  "", "", DEF_LANG_SPEC },
    { "python",  "py",                              "",                  "", "", DEF_LANG_SPEC },
    { "ruby",    "rb|rhtml|rjs|rxml|erb|rake|spec", "",                  "", "", DEF_LANG_SPEC },
    { "shell",   "sh|bash|csh|tcsh|ksh|zsh",        "",                  "", "", DEF_LANG_SPEC },
    { "sql",     "sql|ctl",                         "",                  "", "", DEF_LANG_SPEC },
    { "tcl",     "tcl|itcl|itk",                    "",                  "", "", DEF_LANG_SPEC },
    { "tex",     "tex|cls|sty",                     "",                  "", "", DEF_LANG_SPEC },
	{ "make",    "mk|make",                         "Makefile|makefile", "", "", {1, 1, 0, 0, CASE_SENS,  CASE_SENS,  CASE_INSENS}},
	{ "makeall", "in|mk|make|config",               "Makefile|makefile", "", "", {1, 1, 0, 0, CASE_SENS,  CASE_SENS,  CASE_INSENS}},
};

lang_spec_t* langs = NULL; 


void init_langs(void)
{
	if(langs)
		return;

	int tmpl_lengs_num = sizeof(langs_tmpl) / sizeof(lang_spec_t);
	
	langs = ag_calloc( tmpl_lengs_num + EXTA_USER_LANGS , sizeof(lang_spec_t));

	memcpy(langs, langs_tmpl, sizeof(langs_tmpl));
}

#define GET_SENS(NAME, _str)                                                                      \
do {                                                                                              \
	(_str)++;                                                                                     \
	if(*(_str) == 'y')      {cur->prop. NAME ## _sens = 1; str+=2;}                               \
	else if(*(_str) == 'n') {cur->prop. NAME ## _sens = 0; str+=2;}                               \
	else die("Wrong 'type-add/set' format: sens - expected 'y' or 'n', but getting %c", *(_str)); \
}while(0) 

#define GET_PATTERN(NAME, _str, _tmp)                   \
do{                                                     \
	if(cur->prop. NAME ## set)                          \
	{                                                   \
		int d = strlen(cur-> NAME );                    \
		int s = strlen((_str));                         \
		if(MAX_LANG_RULE_STRING <= (d + s + 1))         \
			die("'type-add/set' pattren is too long");  \
		cur-> NAME [d] = '|';                           \
		_tmp = cur-> NAME + d + 1;                      \
	}                                                   \
	else                                                \
		cur->prop. NAME ## set = 1;                     \
}while(0)

#define COPY_PATTERN(_tmp, _str)                        \
do{                                                     \
	while ((*_tmp++ = * _str++) != 0)                   \
		if(*(_tmp - 1) == ',') *(_tmp - 1) = '|';       \
}while(0);

#define PARSE_COLON(_tmp, _buf, _str, _ok)              \
do{                                                     \
	_buf[0] = 0;                                        \
	_tmp = _buf;                                        \
	while(*_str)                                        \
	{                                                   \
		if(*_str == ':')                                \
		{                                               \
			*_tmp = 0;                                  \
			_ok   = 1;                                  \
			break;                                      \
		}                                               \
		*_tmp++ = *_str++;                              \
	}                                                   \
	if(!_ok || !*_buf || !*_str)                        \
		die("'type-add/set' - wrong type definition");  \
}while(0)

int name_to_index(const char* name)
{
	int idx;
	for(idx = 0; idx < get_lang_count(); idx++)
	{
		if(!strcmp(name, langs[idx].name))
		{
			return idx;
		}
	}
	return -1;
	
}

char* type_add(char* str, int set)
{
	char         buf[MAX_LANG_RULE_STRING];
	int          OK      = 0;
	lang_spec_t* cur     = NULL;
	char*        tmp_str = buf;
	int          idx     = 0;
	
	init_langs();

	if(added_langs_ctr >= EXTA_USER_LANGS)
		die("'type-add/set' - number of user added languages cannot exceed max (%d)", added_langs_ctr, EXTA_USER_LANGS); 

	PARSE_COLON(tmp_str, buf, str, OK);
	
	if(strlen(buf) >= MAX_LANG_NAME_SIZE)
		die("'type-add/set' - type name is too long");
	
	for(idx = 0; idx < get_lang_count(); idx++)
	{
		if(!strcmp(buf, langs[idx].name))
		{
			cur = langs + idx;
		}
	}
	if(!cur)
	{
		cur = langs + get_lang_count(); 
		added_langs_ctr++;
		strcpy(cur->name, buf);

	}
	else if(set)
	{
		memset(cur, 0, sizeof(lang_spec_t));
		strcpy(cur->name, buf);
	}
		
	str++;
	
	PARSE_COLON(tmp_str, buf, str, OK);
	
	if(!strcmp(buf, "is"))
	{
		tmp_str = cur->is;
		GET_SENS(is, str);
		GET_PATTERN(is, str, tmp_str);
		COPY_PATTERN(tmp_str, str);
	}
	else if(!strcmp(buf, "match"))
	{
		tmp_str = cur->match;
		GET_SENS(match, str);
		GET_PATTERN(match, str, tmp_str);
		COPY_PATTERN(tmp_str, str);
	}
	else if(!strcmp(buf, "ext"))
	{
		tmp_str = cur->ext;
		GET_SENS(ext , str);
		GET_PATTERN(ext, str, tmp_str);
		COPY_PATTERN(tmp_str, str);
	}
	else if(!strcmp(buf, "regex"))
    {
		str++;
		tmp_str = cur->regex;
		GET_PATTERN(regex, str, tmp_str);
		strcpy(tmp_str, str);
    }
	else
		die("'type-add/set' - wrong pattern type (%s) should be one from  [is|match|ext|regex]", buf);
	
	return cur->name;
}


int get_lang_count() 
{
	init_langs();
    return sizeof(langs_tmpl) / sizeof(lang_spec_t) + added_langs_ctr;
}

//char* tmp = "(^(absd|dfg)$)|\\.(hhh|in)$|(?i)(kkk|ggg)(?-i)";
//char* tmp_is    = "(^((?i)absd|klmn(?-i)|dfg(?-i))$)";
//char* tmp_ext   = "(\\.((?i)hhh|bbb(?-i)|in(?-i)|(?i)vv(?-i))$)";
//char* tmp_match = "((?i)kkk(?-i)|(?-i)hhh|(?i)bbb(?-i))";
//(\.((?!k)|(?!c|d)|e)$)|(?!(aaaa|bbbb))|(kkk|ggg)|^((?!abcd)|rtye)$
//(?<!\.mod)(\.c)$
//(?i)(?<!\.mod)(\.c|H)(?-i)$
	
#define TMP_REG_CAPACITY   1024

#define START_LEN_IS    3
#define START_LEN_EXT   4
#define START_LEN_MATCH 1
#define START_LEN_REGEX 1

#define REGEX_INSENS_SET        "(?i)"
#define REGEX_INSENS_SET_LEN    4

#define REGEX_SENS_SET          "(?-i)"
#define REGEX_SENS_SET_LEN      5

#define REG_RSRV                20

static inline int fill_pattern(char* regexp, int len, char* pattern, int* fl, int sens)
{
	int ad_len = strlen(pattern);

	if(TMP_REG_CAPACITY <= (len + ad_len + REG_RSRV))
	{
		die("Too long pattern"); 
	}

	if(!*fl)
		*fl = 1;
	else
		regexp[len++] = '|';
		
	if(sens== CASE_INSENS)
	{
		strcpy(regexp + len, REGEX_INSENS_SET);
		len += REGEX_INSENS_SET_LEN;
	}
	else
	{
		strcpy(regexp + len, REGEX_SENS_SET);
		len += REGEX_SENS_SET_LEN;
	}
	
	strcpy(regexp + len, pattern);
	len+=ad_len;
	strcpy(regexp + len, REGEX_SENS_SET);
	len += REGEX_SENS_SET_LEN;
	
	return len;
}

char* make_lang_regex(int* lang_index, int size)
{
	int i,tmp;
	static char tmp_is[TMP_REG_CAPACITY]    = {"(^("};   //close ")$)"
	static char tmp_ext[TMP_REG_CAPACITY]   = {"(\\.("}; //close ")$)"
	static char tmp_match[TMP_REG_CAPACITY] = {"("};     // close ")"
	static char tmp_regex[TMP_REG_CAPACITY] = {"("};     // close ")"
	
	int f_is    = 0;
	int f_ext   = 0;
	int f_match = 0;
	int f_regex = 0;	

	int len_is    = START_LEN_IS;
	int len_ext   = START_LEN_EXT;
	int len_match = START_LEN_MATCH;
	int len_regex = START_LEN_REGEX;
	
	lang_spec_t* cur;
	int len = 0;
		
	for(i = 0; i < size; i++)
	{
		if( lang_index[i] >= get_lang_count())
			die("make_lang_regex: lang index great then lang number"); 
		cur = langs + lang_index[i];

		if(cur->prop.isset)
			len_is = fill_pattern(tmp_is, len_is, cur->is, &f_is, cur->prop.is_sens);
		
		if(cur->prop.extset)
			len_ext = fill_pattern(tmp_ext, len_ext, cur->ext, &f_ext, cur->prop.ext_sens);
		
		if(cur->prop.matchset)
			len_match = fill_pattern(tmp_match, len_match, cur->match, &f_match, cur->prop.match_sens);
		
		if(cur->prop.regexset)
		{
			if(f_regex)
				tmp_regex[len_regex++] = '|';
			else
				f_regex = 1;
			
			tmp = strlen(cur->regex);
		
			if(TMP_REG_CAPACITY <= (len_regex + tmp + REG_RSRV))
			{
				//return -1; TBD
			}
			strcpy(tmp_regex+len_regex, cur->regex);

			len_regex += tmp;
			strcpy(tmp_regex+len_regex,")");
			len_regex += 1;
		}
	}
	
	char *regex = ag_malloc(len_is + len_ext + len_match + len_regex + 20);

	if(f_is)
	{
		strcpy(regex + len, tmp_is);
		len += len_is;
		strcpy(regex+len, ")$)");
		len += 3;
		if(f_ext || f_match || f_regex)
			regex[len++] = '|';
	}
	
	if(f_ext)
	{
		strcpy(regex + len, tmp_ext);
		len += len_ext;
		strcpy(regex + len,")$)");
				len += 3;
		if(f_match || f_regex)
			regex[len++] = '|';
	}
	
	if(f_match)
	{
		strcpy(regex + len, tmp_match);
		len += len_match;
		regex[len++] = ')';
		if(f_regex)
			regex[len++] = '|';
	}
	
	if(f_regex)
		strcpy(regex + len, tmp_regex);

	log_msg("File search regular expression is %s\n" , regex);

	return regex;
}

