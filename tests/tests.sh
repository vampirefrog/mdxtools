#!/bin/sh

MML2MDX=../mml2mdx
MDXDUMP=../mdxdump

for i in *.mml
do
	BASE=$(basename "$i" .mml)
	MML2MDX_MDX="$BASE.mml2mdx.mdx"
	rm -f $MML2MDX_MDX
	MXC_MDX="$BASE.mdx"
	if ! $MML2MDX "$i" -o "$MML2MDX_MDX"
	then
		echo "Error $? while compiling"
		exit 1
	fi
	if [ ! -f "$MML2MDX_MDX" ]
	then
		echo "$MML2MDX_MDX: No file was compiled"
		exit 1
	fi

	mml2mdxsum=$(md5sum "$MML2MDX_MDX" | awk '{ print $1 }')
	mxcsum=$(md5sum "$MXC_MDX" | awk '{ print $1 }')
	if [ x$mml2mdxsum = x$mxcsum ]
	then
		echo -e "\033[32mPASS\033[0m $i"
		rm -f "$MML2MDX_MDX";
	else
		echo -e "\033[31mFAIL\033[0m $i"
		$MDXDUMP $MML2MDX_MDX > $MML2MDX_MDX.dump
		$MDXDUMP $MXC_MDX > $MXC_MDX.dump
		diff $MML2MDX_MDX.dump $MXC_MDX.dump
		exit 1
	fi
done

rm -f *.dump
rm -f *.mml2mdx.mdx
