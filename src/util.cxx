#include "UserInterface.h"

void
UserInterface::show_page( int n )
{
    for (int i = 0; i < sizeof(page)/sizeof(page[0]); ++i)
	if (i == n)
	    page[i]->show();
	else
	    page[i]->hide();
}

int
UserInterface::set_choice( Fl_Choice* c, const char* s )
{
    const Fl_Menu_Item* m = c->menu();
    int i = 0;
    int nest = 0;
    for (i = 0; i < m->size(); ++i)
    {
	if (m[i].submenu())
	{
	    ++nest;
	}
	else if (m[i].label() != 0)
	{
	    if (strcmp(s, m[i].label()) == 0)
	    {
		c->value(i);
		return 1;
	    }
	}
	else
	{
	    --nest;
	}
    }
    return 0;
}

