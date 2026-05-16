/*
 * gui.h -- GUI function access
 */

#ifndef GUI_H
#define GUI_H

typedef struct app App;
App* setupUI(void);
void runUI(void);
void teardownUI(App* app);

#endif /* GUI_H */

