#!/bin/bash

set -e

read_TRANS_calls() {
  declare -n arr="$1"
  for file in $(find $2 -type f -name '*.cpp'); do
    while IFS='' read -r line; do
    arr+=( "$line" )
    done < <(sed --regexp-extended --null-data 's/"\r?\n *"//g' $file | grep --only-matching --no-filename --perl-regexp 'TRANS *\("\K[^"]*(?="\))')
  done
}

source_translatable=()
read_TRANS_calls source_translatable Source 

all_translatable=("${source_translatable[@]}")
read_TRANS_calls all_translatable JUCE

found_problems=0

for translation in Translations/*; do
  echo "== Checking $translation =="
  missing=$(comm -13  \
   <(grep --only-matching --perl-regexp '^"\K[^"]*' $translation | sort -u) \
   <(printf '%s\n' "${source_translatable[@]}" | sort -u))

  if [[ ! -z "$missing" ]]; then
    echo "== Missing translations, add them =="
    echo "$missing"
    found_problems=1
  fi

 redundant=$(comm -23  \
   <(grep --only-matching --perl-regexp '^"\K[^"]*' $translation | sort -u) \
   <(printf '%s\n' "${all_translatable[@]}" | sort -u))
  if [[ ! -z "$redundant" ]]; then
    echo "== Redundant translations, delete them =="
    echo "$redundant"
    found_problems=1
  fi
done

exit $found_problems
