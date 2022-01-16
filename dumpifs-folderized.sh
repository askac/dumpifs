#!/bin/bash
# Author: bertelsmann
# Original: https://turbo-quattro.com/showthread.php?15648-How-to-extract-a-IFS-file
# Link: https://turbo-quattro.com/showthread.php?15648-How-to-extract-a-IFS-file&p=367809&viewfull=1#post367809
# Modified by: @sickcodes <https://github.com/sickcodes/>

ELEVATED="${ELEVATED:=false}"
IFSDUMP=dumpifs
TMPDIR="${PWD}"
CURDIR="${PWD}"
IMAGE="${1}"

if ! [ -e "${IMAGE}" ]; then
    echo "Usage ./script.sh <qnxifs>"
    exit 1
fi

echo "Dumping contents of ${IMAGE}..."
${IFSDUMP} -z "${IMAGE}" | awk '{print $3}' > "${TMPDIR}"/ifscont.txt
${IFSDUMP} -z "${IMAGE}" | grep "\->" | awk '{print $3" -> "$5}' > "${TMPDIR}"/ifslinks.txt

while read -r i; do
    dirname "${i}"
done < ifscont.txt > "${TMPDIR}"/dirlist.txt

sort -o "${TMPDIR}"/dirlist.txt "${TMPDIR}"/dirlist.txt

while read -r i; do
    #create symlinks
    mkdir -p ./"${i}"
done < dirlist.txt

while read -r i; do
    SRC="$(echo -e "$i" | awk '{print $3}')"
    LNK="$(echo -e "$i" | awk '{print $1}')"
    LNKDIR="$(dirname "${LNK}")"
    LNKBASE="$(basename "${LNK}")"
    cd "${LNKDIR}" || exit
    ln -s "${SRC}" "${LNKBASE}"
    cd "${CURDIR}" || exit
done < "${TMPDIR}"/ifslinks.txt

if [ "${ELEVATED}" == true  ]; then
    sudo ${IFSDUMP} -zx "${IMAGE}"
else
    ${IFSDUMP} -zx "${IMAGE}"
fi

echo "Folder tree created."