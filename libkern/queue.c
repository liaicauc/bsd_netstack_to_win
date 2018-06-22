#include <sys/param.h>
#include <sys/proc.h>

/*
* insert an element into a queue
*/
#undef insque
_insque(
	register struct prochd *element, 
	register struct prochd*head)
{
	element->ph_link = head->ph_link;
	head->ph_link = (struct proc *)element;
	element->ph_rlink = (struct proc *)head;
	((struct prochd *)(element->ph_link))->ph_rlink = (struct proc *)element;
}

/*
* remove an element from a queue
*/
#undef remque
_remque(element)
register struct prochd *element;
{
	((struct prochd *)(element->ph_link))->ph_rlink = element->ph_rlink;
	((struct prochd *)(element->ph_rlink))->ph_link = element->ph_link;
	element->ph_rlink = (struct proc *)0;
}