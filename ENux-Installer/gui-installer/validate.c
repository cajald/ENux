/*
 * validate.c -- validate all inputs before install
 */

#include <stdbool.h>
#include <string.h>

#include <ui.h>

#include "gui.h"

#define EMPTY(s) ((s) == NULL || *(s) == '\0')

static bool
validateUserPage(GUI* app)
{
	char *user    = uiEntryText(app->userEnt);
	char *pass    = uiEntryText(app->passEnt);
	char *passRep = uiEntryText(app->repPassEnt);

	bool ok = true;

	if (EMPTY(user)) {
		uiLabelSetText(app->status, "User is missing");
		ok = false;
	}
	else if (EMPTY(pass)) {
		uiLabelSetText(app->status, "Password is missing");
		ok = false;
	}
	else if (strcmp(pass, passRep) != 0) {
		uiLabelSetText(app->status, "Passwords don't match");
		ok = false;
	}

	if (ok) {
		uiLabelSetText(app->status, "User data valid");
	}

	app->allowNext = ok;

	uiFreeText(user);
	uiFreeText(pass);
	uiFreeText(passRep);

	return ok;
}

void
nextCb(GUI* app)
{
	switch (app->page) {
	case PAGE_USER:
		app->allowNext = validateUserPage(app);
		break;
	}
}

void
onAnyInputChanged(uiEntry* e, void* data)
{
	(void)e;
	GUI* app = (GUI*)data;
	nextCb(app);
}
