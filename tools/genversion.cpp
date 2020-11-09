//       _________ __                 __
//      /   _____//  |_____________ _/  |______     ____  __ __  ______
//      \_____  \\   __\_  __ \__  \\   __\__  \   / ___\|  |  \/  ___/
//      /        \|  |  |  | \// __ \|  |  / __ \_/ /_/  >  |  /\___ |
//     /_______  /|__|  |__|  (____  /__| (____  /\___  /|____//____  >
//             \/                  \/          \//_____/            \/
//  ______________________                           ______________________
//                        T H E   W A R   B E G I N S
//         Stratagus - A free fantasy real time strategy game engine
//
/**@name genversion.cpp - Generate Stratagus header file version-generated.h */
//
//      (c) Copyright 2013 by Pali Rohár <pali.rohar@gmail.com>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; only version 2 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//      02111-1307, USA.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>

/* usage: genversion "/path/to/version-generated.h" "major.minor.path.patch2" */

int main(int argc, char * argv[]) {

	FILE * file;
	int old_ver[4] = { -1, -1, -1, -1 };
	int new_ver[4] = { -1, -1, -1, -1 };
	char* old_rev = (char*)calloc(sizeof(char), 1024);

	if ( argc != 3 )
		return 1;

	if ( sscanf(argv[2], "%d.%d.%d.%d", &new_ver[0], &new_ver[1], &new_ver[2], &new_ver[3]) != 4 ) {
		new_ver[3] = 0;
		if ( sscanf(argv[2], "%d.%d.%d", &new_ver[0], &new_ver[1], &new_ver[2]) != 3 )
			return 1;
	}

	file = fopen(argv[1], "r");
	if ( file ) {
		if ( fscanf(file, "/* %d %d %d %d %s */", &old_ver[0], &old_ver[1], &old_ver[2], &old_ver[3], old_rev) != 5 ) {
			old_ver[0] = -1;
			old_ver[1] = -1;
			old_ver[2] = -1;
			old_ver[3] = -1;
		}
		fclose(file);
	}

	file = fopen(".git/HEAD", "r");
	char *git_rev, *gitrevfile;
	if ( file ) {
		git_rev = (char*)calloc(sizeof(char), 1024);
		if (fscanf(file, "ref: %s", git_rev) != 1 ) {
			fscanf(file, "%s", git_rev);
		}
		fclose(file);
		gitrevfile = (char*)calloc(sizeof(char), strlen(git_rev) + 6);
		sprintf(gitrevfile, ".git/%s", git_rev);
		file = fopen(gitrevfile, "r");
		free(gitrevfile);
		if (file) {
			git_rev = (char*)calloc(sizeof(char), 1024);
			fscanf(file, "%s", git_rev);
			fclose(file);
		}
	} else {
		git_rev = NULL;
		fprintf(stderr, "Cannot get git revision\n");
	}

	bool needs_update = false;
	for (int i = 0; i < 4; i++) {
		if (old_ver[i] != new_ver[i]) {
			printf("%d %d %d %d - %d %d %d %d\n", old_ver[0], old_ver[1], old_ver[2], old_ver[3], new_ver[0], new_ver[1], new_ver[2], new_ver[3]);
			puts("ver update");
			needs_update = true;
			break;
		}
	}
	if (git_rev == NULL) {
		needs_update = true;
	}

	if (!needs_update) {
		needs_update = strcmp(old_rev, git_rev);
	}

	if (!needs_update) {
		puts("[genversion] no update needed");
		return 0;
	}
	puts("[genversion] update needed");

	file = fopen(argv[1], "w");
	if ( ! file )
		return 1;

	if (git_rev == NULL) {
		fprintf(file, "/* %d %d %d %d no-rev */\n", new_ver[0], new_ver[1], new_ver[2], new_ver[3]);
	} else {
		fprintf(file, "/* %d %d %d %d %s */\n", new_ver[0], new_ver[1], new_ver[2], new_ver[3], git_rev);
	}
	fprintf(file, "/* This file is autogenerated, do not modify it! */\n");
	fprintf(file, "#define StratagusMajorVersion %d\n", new_ver[0]);
	fprintf(file, "#define StratagusMinorVersion %d\n", new_ver[1]);
	fprintf(file, "#define StratagusPatchLevel %d\n", new_ver[2]);
	fprintf(file, "#define StratagusPatchLevel2 %d\n", new_ver[3]);

	if ( git_rev != NULL )
		fprintf(file, "#define StratagusGitRev %s\n", git_rev);

	fclose(file);
	return 0;

}
