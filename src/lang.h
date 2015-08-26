#ifndef LANG_H
#define LANG_H

#define MAX_LANG_RULE_STRING	512
#define MAX_LANG_NAME_SIZE		32
#define EXTA_USER_LANGS			32

typedef enum {
	CASE_SENS,
	CASE_INSENS 
}casesens_t;

typedef struct 
{
	int extset;
	int isset;
	int matchset;
	int regexset;
	
	casesens_t 	ext_sens;
	casesens_t	is_sens;
	casesens_t 	match_sens;
}lang_properties;

typedef struct 
{
 	char	name[MAX_LANG_NAME_SIZE]; 
 	char 	ext[MAX_LANG_RULE_STRING];
	char 	is[MAX_LANG_RULE_STRING];
	char 	match[MAX_LANG_RULE_STRING];
	char 	regex[MAX_LANG_RULE_STRING];
	lang_properties prop;
} lang_spec_t;

extern lang_spec_t* langs;

int 	name_to_index(const char* name);
void 	init_langs(void);
int		get_lang_count(void);
char* 	make_lang_regex(int* lang_index, int len);
char* 	type_add(char* str, int set);

#endif
