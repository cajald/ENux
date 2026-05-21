/*
 * validate.c -- validate inputs before install
 */

#include <stdbool.h>
#include <string.h>
#include <ui.h>

#include "gui.h"

#define EMPTY(s) ((s) == NULL || *(s) == '\0')

static bool
validateUser(GUI* app)
{
	char *user    = uiEntryText(app->userEnt);
	char *pass    = uiEntryText(app->passEnt);
	char *passRep = uiEntryText(app->repPassEnt);
	char *rootPass = uiEntryText(app->rootPassEnt);
	char *rootPassRep = uiEntryText(app->repRootPassEnt);

	bool ok = true;

	if (EMPTY(user)) {
		uiLabelSetText(app->status, "User is missing");
		ok = false;
	}
	else if (EMPTY(pass)) {
		uiLabelSetText(app->status, "Password is missing");
		ok = false;
	}
	else if (EMPTY(rootPassRep)) {
		uiLabelSetText(app->status, "Root password is missing");
		ok = false;
	}
	else if (strcmp(rootPass, rootPassRep) != 0) {
		uiLabelSetText(app->status, "Passwords do not match");
		ok = false;
	}
	else if (strcmp(pass, passRep) != 0) {
		uiLabelSetText(app->status, "Passwords do not match");
		ok = false;
	}
	else {
		uiLabelSetText(app->status, "User data OK");
	}

	uiFreeText(user);
	uiFreeText(pass);
	uiFreeText(passRep);

	return ok;
}

static bool
validateDisk(GUI* app)
{
	int disk = uiComboboxSelected(app->diskSelect);
	int fs   = uiComboboxSelected(app->fsType);

	if (disk < 0) {
		uiLabelSetText(app->status, "No disk selected");
		return false;
	}

	if (fs < 0) {
		uiLabelSetText(app->status, "No filesystem selected");
		return false;
	}

	return true;
}

static bool
validateInstall(GUI* app)
{
	(void)app;
	return true;
}

void
validateAll(GUI* app)
{
	bool ok = true;

	switch (app->page) {

		case PAGE_WELCOME:
			ok = true;
			uiLabelSetText(app->status, "Ready");
			break;

		case PAGE_DISK:
			ok = validateDisk(app);
			break;

		case PAGE_USER:
			ok = validateUser(app);
			break;

		case PAGE_INSTALL:
			ok = validateInstall(app);
			break;

		default:
			ok = false;
			break;
	}

	app->allowNext = ok;
	updateNav(app);
}

void
onEntryChanged(uiEntry* e, void* data)
{
	(void)e;
	validateAll((GUI*)data);
}

void
onComboChanged(uiCombobox* c, void* data)
{
	(void)c;
	validateAll((GUI*)data);
}

void
onSpinChanged(uiSpinbox* s, void* data)
{
	(void)s;
	validateAll((GUI*)data);
}

void
onCheckToggled(uiCheckbox* c, void* data)
{
	GUI* app = data;

	int checked = uiCheckboxChecked(c);

	if (checked)
		uiControlEnable(uiControl(app->swapSize));
	else
		uiControlDisable(uiControl(app->swapSize));

	validateAll(app);
}

void
nextCb(GUI* app)
{
	validateAll(app);
	if (!app->allowNext)
		return;
	if (app->page < PAGE_INSTALL)
		app->page++;
	uiTabSetSelected(app->tab, app->page);

	validateAll(app);
}
