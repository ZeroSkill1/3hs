#!/usr/bin/env perl

use warnings;
use strict;
use utf8;

# ====================== #

my $build_dir = "build";
my $lang_dir = "lang";

# NOTE: The first language in this array
#       will be used as the base language.
my @languages = qw(
	english
	dutch
	german
	spanish
	french
	fr_canada
	romanian
	italian
	portuguese
	korean
	greek
	polish
	hungarian
	japanese
	russian
	spearglish
	ryukyuan
	latvian
	jp_osaka
	moldovan
);

# Strings never to put between quotes
my @preserve_keywords = qw(
	PAGE_3HS
	PAGE_THEMES
	UI_GLYPH_A
	UI_GLYPH_B
	UI_GLYPH_X
	UI_GLYPH_Y
	UI_GLYPH_L
	UI_GLYPH_R
	UI_GLYPH_DPAD
);

# ====================== #

-d $lang_dir or die "no language directory available.";
-d $build_dir or die "no build directory available.";

my $source_file = "";
my $header_file = "";

my $langs_w_missing = 0;
my $total_missing = 0;

my @string_ids = qw();

sub parse_lang_file {
	my ($lang_name) = @_;
	open my $fh, "$lang_dir/$lang_name" or die "failed to open $lang_name file.";
	my $is_reference_lang = @string_ids == 0;
	my %strings;
	my $line;

	my $native_name = "";
	my $string_content = "";
	my $current_id = "";

	print "$lang_name ... ";

	while(($line = <$fh>)) {
		chomp $line;
		if ($line =~ /^\s*#/) {
			next;
		} elsif($line =~ /^- /) {
			if($current_id eq "native_name") {
				$native_name = $string_content;
			} elsif($current_id) {
				$strings{$current_id} = $string_content;
			}
			$string_content = "";
			$current_id = $line;
			$current_id =~ s/^- //;
		}
		elsif($line =~ /[^\s]/) {
			$string_content and $string_content .= "\n";
			$string_content .= $line;
		}
	}

	close $fh;

	$current_id and $strings{$current_id} = $string_content;

	if($is_reference_lang) {
		print "reference\n";
		foreach my $k (sort keys %strings) {
			push @string_ids, $k;
		}
	} else {
		my $inval_count = 0;
		foreach my $k (keys %strings) {
			if(not grep /^$k$/, @string_ids) {
				if(not $inval_count) {
					print "invalid id(s): ";
				} else {
					print ", ";
				}
				print $k;
				++$inval_count;
			}
		}
		if($inval_count) {
			print "\n";
			return 0;
		}
	}

	if(not $native_name) {
		print "no native name\n";
		return 0;
	}

	my $missing = 0;

	$source_file .= "\t[lang::$lang_name] =\n\t\t{\n";
	foreach my $k (@string_ids) {
		my $string = $strings{$k};
		if($string) {
			$source_file .= "\t\t\t[str::$k] =";

			open my $lr, '<', \$string;
			my $line;
			while(($line = <$lr>)) {
				$line =~ s/\n$/\\n/;
				$line =~ s/"/\\"/g;
				$line = " \"$line\"";
				foreach my $kw (@preserve_keywords) {
					$line =~ s/$kw/" $kw "/g;
				}
				$source_file .= $line;
			}
			$source_file .= ",\n";
		}
		else {
			$source_file .= "\t\t\tSTUB($k),\n";
			++$missing;
		}
	}
	$source_file .= "\t\t},\n";

	if(not $is_reference_lang) {
		if($missing) {
			print "\033[31;1m$missing\033[0m missing\n";
			$total_missing += $missing;
			++$langs_w_missing;
		} else {
			print "complete\n";
		}
	}

	return $native_name;
}

$source_file .= <<EOF;
/* this file was generated by ./lang/make.pl
 * DO NOT EDIT */

#include <ui/base.hh> /* for UI_GLYPH_* */
#include "./i18n_tab.hh"

#ifndef HS_SITE_LOC
	#define PAGE_3HS "(unset)"
	#define PAGE_THEMES "(unset)"
#else
	#define PAGE_3HS HS_SITE_LOC "/3hs"
	#define PAGE_THEMES HS_SITE_LOC "/wiki/theme-installation"
#endif

#define STUB(id) [str::id] = lang_strtab[lang::english][str::id]

// [str::xxx] is a GCC extension
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
static const char *lang_strtab[lang::_i_max][str::_i_max] =
{
EOF

$header_file .= <<EOF;
/* this file was generated by ./lang/make.pl
 * DO NOT EDIT */

#ifndef inc_i18n_tab_hh
#define inc_i18n_tab_hh

namespace lang
{
	enum _enum
	{

EOF

my @names_ids;
my @lang_names;
my @names;

for my $i (0..$#languages) {
	my $native_name = parse_lang_file "$languages[$i]";
	if($native_name) {
		push @names, $native_name;
		my $up = uc $languages[$i];
		push @names_ids, "LANGNAME_$up";
		push @lang_names, "$languages[$i]";
		$header_file .= <<EOF;
		$languages[$i] = $i,
EOF
	}
}

$header_file .= <<EOF;
		// === //
		_i_max,
	};

	using type = unsigned short int;
}

namespace str
{
	enum _enum
	{
EOF

for my $i (0..$#string_ids) {
	$header_file .= <<EOF
		$string_ids[$i] = $i,
EOF
}

$header_file .= <<EOF;
		// === //
		_i_max,
	};

	using type = unsigned short int;
}

EOF

for my $i (0..$#names) {
	$header_file .= <<EOF;
#define $names_ids[$i] "$names[$i]"
EOF
}

$header_file .= <<EOF;

namespace i18n
{
	const char *langname(lang::type);
}

EOF

$source_file .= <<EOF;
};
#pragma GCC diagnostic pop

const char *i18n::langname(lang::type id)
{
	switch(id)
	{
EOF

foreach my $lang (@lang_names) {
	my $def = uc $lang;
	$def = "LANGNAME_$def";
	$source_file .= "\tcase lang::$lang: return $def;\n";
}

$source_file .= <<EOF;
	}
	return "invalid";
}
EOF

$header_file .= <<EOF;
#define I18N_ALL_NAMES \\
EOF

foreach my $name (@names_ids) {
	$header_file .= "\t$name, \\\n";
}

$header_file .= <<EOF;

#define I18N_ALL_IDS \\
EOF

foreach my $id (@lang_names) {
	$header_file .= "\tlang::$id, \\\n";
}

$header_file .= <<EOF;

#endif
EOF

my $old_header = "";
my $rheader;
open $rheader, "<", "$build_dir/i18n_tab.hh" and do {
	local $/;
	$old_header = <$rheader>;
};

# Try to not write the header to save us a recompile if we just changed a single string
$old_header eq $header_file or do {
	my $header;
	open $header, ">", "$build_dir/i18n_tab.hh" or die "failed to write i18n_tab.hh";
	print $header $header_file;
	close $header;
};

open my $source, ">", "$build_dir/i18n_tab.cc" or die "failed to write i18n_tab.cc";
print $source $source_file;
close $source;

if($langs_w_missing) {
	print "missing $total_missing strings in $langs_w_missing languages\n";
} else {
	print "translations are complete\n"
}

