/*
 * gui.h -- GUI function access
 */

#ifndef GUI_H
#define GUI_H

typedef struct GUI GUI;
GUI* setupUI(void);
void runUI(void);
void teardownUI(GUI* app);

#endif /* GUI_H */

