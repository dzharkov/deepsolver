/*
 *
 * This file was taken from rpm sources. It provides declaration of
 * rpmsetcmp() function required for set versions comparing during
 * experiments. Everything looks like librpm.so contains rpmsetcmp()
 * implementation but header files from librpm-devel do not include its
 * declaration.
 *
 * msp@, 2011-08-24
 *
 */

#ifndef SET_H
#define SET_H

/* Compare two set-versions.
 * Return value:
 *  1: set1  >  set2
 *  0: set1 ==  set2
 * -1: set1  <  set2 (aka set1 \subset set2)
 * -2: set1 !=  set2
 * -3: set1 decoder error
 * -4: set2 decoder error
 * For performance reasons, set1 should come on behalf of Provides.
 */
int rpmsetcmp(const char *set1, const char *set2);

/*
 * API for creating set versions.
 */

// initialize new set
struct set *set_new(void);

// add new symbol to set
void set_add(struct set *set, const char *sym);

// make set-version
const char *set_fini(struct set *set, int bpp);

// free set
struct set *set_free(struct set *set);

#endif
