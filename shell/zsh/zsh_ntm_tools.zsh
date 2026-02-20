# ===============================
# zsh-ntm-tool function
# ===============================

function nocolor(){
    "$@" | sed -E s'/\x1b\[[0-9;]*[a-zA-Z]?//'g
}


function fview() {
    local input=$(cat)  # legge tutto l'input una sola volta
    local line=$(echo "$input" | bat --color=always --paging=always --wrap=never --pager="less -RSj1 +1" | fzf --ansi | awk '{print $1}')
    [ -z "$line" ] && return
    echo "$input" | bat --color=always --paging=always --wrap=never --highlight-line "$line" --pager="less -RSj1 +$line"
}


function ntm() {
    local output current_file base_file 
    local idx target 
    local use_temp=0

    if [[ "$1" =~ ^[0-9]+$ ]]; then
        use_temp=1
        target=$1
        shift

        output=$(nocolor ntm view file)
        current_file=$(echo "$output" | grep -m1 "Notes file in use:" | sed 's/Notes file in use: //' | sed 's/\.\.\./ /')
        base_file=$(basename "$current_file")
        idx=$(echo "$output" | awk -v f="$base_file" '$0 ~ f {print $1; exit}')

        command ntm setting "$target"
    fi  
 
    local has_f=0
    for arg in "$@"; do
        if [[ "$arg" == -* && "$arg" == *f* ]]; then
            has_f=1
            break
        fi
        done

        if [[ -t 1 ]]; then
            if [[ "$1" == "view" && "$2" == "note" ]]; then
                command ntm "$@" | bat --color=always --paging=always --wrap=never --pager="less -RSj1 +1"
            elif [[ "$1" == "find" && $has_f -eq 0 ]]; then
                command ntm "$@" | bat --color=always --paging=always --wrap=never --pager="less -RSj1 +1"
            else
                command ntm "$@"
            fi
        else
        command ntm "$@"
    fi

    if [[ $use_temp -eq 1 ]]; then
        command ntm setting "$idx"
    fi
}
