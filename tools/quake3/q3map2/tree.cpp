/* -------------------------------------------------------------------------------

   Copyright (C) 1999-2007 id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ----------------------------------------------------------------------------------

   This code has been altered significantly from its original form, to support
   several games based on the Quake III Arena engine, in the form of "Q3Map2."

   ------------------------------------------------------------------------------- */



/* dependencies */
#include "q3map2.h"




void RemovePortalFromNode( portal_t *portal, node_t *l );

node_t *NodeForPoint( node_t *node, const Vector3& origin ){
	while ( node->planenum != PLANENUM_LEAF )
	{
		if ( plane3_distance_to_point( mapplanes[node->planenum].plane, origin ) >= 0 ) {
			node = node->children[0];
		}
		else{
			node = node->children[1];
		}
	}

	return node;
}



/*
   =============
   FreeTreePortals_r
   =============
 */
void FreeTreePortals_r( node_t *node ){
	portal_t    *p, *nextp;
	int s;

	// free children
	if ( node->planenum != PLANENUM_LEAF ) {
		FreeTreePortals_r( node->children[0] );
		FreeTreePortals_r( node->children[1] );
	}

	// free portals
	for ( p = node->portals; p; p = nextp )
	{
		s = ( p->nodes[1] == node );
		nextp = p->next[s];

		RemovePortalFromNode( p, p->nodes[!s] );
		FreePortal( p );
	}
	node->portals = NULL;
}

/*
   =============
   FreeTree_r
   =============
 */
void FreeTree_r( node_t *node ){
	// free children
	if ( node->planenum != PLANENUM_LEAF ) {
		FreeTree_r( node->children[0] );
		FreeTree_r( node->children[1] );
	}

	// free the node
	delete node;
}


/*
   =============
   FreeTree
   =============
 */
void FreeTree( tree_t& tree ){
	FreeTreePortals_r( tree.headnode );
	FreeTree_r( tree.headnode );
}

//===============================================================

void PrintTree_r( const node_t *node, int depth ){
	for ( int i = 0; i < depth; i++ )
		Sys_Printf( "  " );
	if ( node->planenum == PLANENUM_LEAF ) {
		if ( node->brushlist.empty() ) {
			Sys_Printf( "NULL\n" );
		}
		else
		{
			for ( const brush_t& bb : node->brushlist )
				Sys_Printf( "%d ", bb.original->brushNum );
			Sys_Printf( "\n" );
		}
		return;
	}

	const plane_t& plane = mapplanes[node->planenum];
	Sys_Printf( "#%d (%5.2f %5.2f %5.2f):%5.2f\n", node->planenum,
	            plane.normal()[0], plane.normal()[1], plane.normal()[2],
	            plane.dist() );
	PrintTree_r( node->children[0], depth + 1 );
	PrintTree_r( node->children[1], depth + 1 );
}
