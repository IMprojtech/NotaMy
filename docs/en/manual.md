
---

# 📒 `NotaMy` Documentation

NotaMy (ntm) is a command-line tool for hierarchical management of technical knowledge and work contexts.

It organizes information into note files (contexts) structured as node trees.
Each node may contain:

* an identifying tag
* a short comment
* an extended text body
* searchable keywords
* file references
* a unique hash

The system allows insertion, modification, visualization, and structured search of information directly from the shell.

> NotaMy is designed to support long-term technical workflows, keeping contexts separated and reducing information dispersion.

---

## 📌 SYNOPSIS

```bash
ntm [COMMAND] [SUBCOMMAND] [OPTIONS]  # Full command with subcommand
ntm [COMMAND] [OPTIONS]               # Simple command (e.g., find, help)
```

---

## 🧭 AVAILABLE COMMANDS

| Command    | Subcommand | Description                                                            |
| ---------- | ---------- | ---------------------------------------------------------------------- |
| `add`      | `note`     | Insert a new note                                                      |
|            | `file`     | Add a new note archive file                                            |
| `view`     | `note`     | Display existing notes                                                 |
|            | `file`     | Display available note files                                           |
|            | `tag`      | Show tag hierarchy                                                     |
| `find`     |            | Search one or more notes by criteria (tag, hash, keywords, date, etc.) |
| `modify`   |            | Modify an existing note                                                |
| `organize` |            | Move a note within the hierarchy                                       |
| `remove`   |            | Delete a note                                                          |
| `setting`  |            | Set a note file as active                                              |
| `editor`   |            | Set the text editor (`vim`, `nano`, `nul`)                             |
| `backup`   |            | Create a backup of the active note file                                |
| `help`     |            | Display general help                                                   |

---

## ⚙️ AVAILABLE OPTIONS

> Options vary depending on the command. Common ones are listed below.

### `add note`

| Short Option | Long Option    | Description                                 |
| ------------ | -------------- | ------------------------------------------- |
| `-t`         | `--tag`        | Insert a tag (required, default: `"?"`)     |
| `-c`         | `--comment`    | Add a note comment (required)               |
| `-k`         | `--keywords`   | Insert one or more space-separated keywords |
| `-f`         | `--file`       | Attach a file                               |
| `-b`         | `--body`       | Open the editor to write the note body      |
| `-i`         | `--input`      | Disable editor and allow stdin redirection  |
| `-p`         | `--protection` | Enable note encryption                      |

### `add file`

| Required Argument | Options        |
| ----------------- | -------------- |
| `<path/file>`     | `-c` (comment) |

### `view note`, `view tag`, `view file`

| Short Option | Long Option    | Description                |
| ------------ | -------------- | -------------------------- |
| `-e`         | `--extended`   | Extended view (hash, date) |
| `-b`         | `--body`       | Show note bodies           |
| `-p`         | `--protection` | Include protected notes    |

### `find`

| Option           | Description                                       |
| ---------------- | ------------------------------------------------- |
| `-t`             | Search by tag                                     |
| `-h`             | Search by specific hash                           |
| `-k`             | Search by keywords                                |
| `-d`             | Search by date (relative or absolute)             |
| `-f`             | Open attached file with configured editor         |
| `-o`             | Print attached file content instead of opening it |
| `-e`, `-b`, `-p` | Same as in `view`                                 |

### `modify`

| Required Argument | Options as in `add note` |
| ----------------- | ------------------------ |
| `<hash>`          |                          |

### `organize`

| Required Argument | Destination                                 |
| ----------------- | ------------------------------------------- |
| `<note hash>`     | `"."` (root), `up`, `down`, or another hash |

### `remove`

| Required Option | Description           |
| --------------- | --------------------- |
| `-t`, `-h`      | Remove by tag or hash |

---

## ✍️ INSERTING NOTES

### Basic example

```bash
ntm add note -t "tag1" -c "comment" -k "key1 key2" -f "/path/file.txt" -b
```

> The selected editor will open to write the note body.

### Attaching a file

Use `-f` followed by a full or abbreviated path using `$HOME` or `$(pwd)`:

```bash
ntm add note -t "docs" -c "report" -f "$HOME/reports/report.txt"
```

### Adding keywords

Use `-k` followed by one or more space-separated keywords:

```bash
ntm add note -t "project" -c "description" -k "java API spring"
```

> Keywords improve future searches and are visible in `view note -be` mode.

### Using variables or dynamic commands

Use environment variables and commands to dynamically insert information:

```bash
ntm add note -t "log" -c "recorded on $(date)"
```

### Output redirection

You can redirect command output directly into the note body:

```bash
ls | ntm add note -t "directory" -c "current content" -bi
```

> `-i` is required to read from stdin.

### Direct note nesting

Use special characters `<`, `>` in tags to nest notes:

```bash
ntm add note -t "projects>API" -c "Note inside API"
```

or

```bash
ntm add note -t "API<projects" -c "Note inside API"
```

> Both formats are valid.

### Protecting notes

Use `-p` to protect a note:

```bash
ntm add note -t "security" -c "encrypted content" -p
```

---

## 🔎 VIEWING AND SEARCHING

To provide an efficient user experience, NotaMy focuses on intuitive visualization and powerful search.

### Tag Overview

To display the tag hierarchy:

```bash
ntm view tag
ntm view tag -e   # Also show hashes
```

### List Notes

```bash
ntm view note
```

### Search by tag, hash, keywords, or date

```bash
ntm find -t "projects"
ntm find -h "a1b2"
ntm find -k "API spring"
```

> Customize output with additional options such as `-e`, `-b`, `-p`.

### Time-based search

**Relative** (use `Y`, `M`, `D`, `h`, `m`, `s`):

```bash
ntm find -d 2D    # Last 2 days
```

**Absolute**:

```bash
ntm find -d "2024-05"
ntm find -d "2024-01 to 2024-03"
```

### Expanding or reducing hierarchy view

```bash
ntm find -t "tag1+"    # Extend view to children and descendants
ntm find -t "tag1-"    # Show only siblings
```

> Search is **case-insensitive** and **partial** (prefix is sufficient if unique).

---

## 📂 ATTACHED FILE MANAGEMENT

### Open or print attached files

```bash
ntm find -ft "tag1"
ntm find -fot "tag1"
```

> `-o` is required to write to stdout.

---

## ✏️ MODIFYING AND STRUCTURE MANAGEMENT

### Modify a note

```bash
ntm modify "5b43" -t "new_tag" -c "new comment"
ntm modify "5b43" -b
```

> If the editor is `nul`, the text is overwritten; with `vim/nano`, it is edited in place.

### Reorganize nodes

```bash
ntm organize "5b43" "ff46"
ntm organize "5b43" .
ntm organize "5b43" up
```

### Delete a note

```bash
ntm remove -h "5b43"
```

⚠️ **Warning:** Removal is recursive — all descendants will also be deleted.

---

## ⚙️ SETTINGS

### Set editor

Choose between: `nul`, `vim`, `nano`.

```bash
ntm editor vim
```

### Add archive file

```bash
ntm add file "$HOME/path/new_notes.X" -c "Notes 2024"
```

### View available archive files

```bash
ntm view file
```

### Set active file

```bash
ntm setting 0
```

> The index is visible via `view file`.

---

## 🛡️ BACKUP

```bash
ntm backup
```

Creates a backup copy of the active file.

---

## ❓ HELP

```bash
ntm help
```

---

## 🐚 Zsh Shell

NotaMy includes an optional Zsh wrapper that adds useful functionality without modifying the main program.

The wrapper improves output handling and introduces commands that simplify working with note files and long texts.

---

### Automatic output handling with `bat`

Some commands, for example:

```bash
ntm view note -b
```

may produce long output that is difficult to read directly in the shell.

Normally you would need:

```bash
ntm view note -b | bat
```

The wrapper automatically redirects output to `bat` when appropriate, improving:

* readability
* pagination
* syntax highlighting
* navigation

To force direct stdout output (without `bat`), use:

```bash
ntm view note -b | cat
```

---

### Temporary context switching

The wrapper allows you to use a different note file temporarily without permanently changing the active context.

Standard scenario (assuming you are in file 5):

```bash
ntm setting 1
ntm view note
ntm setting 5
```

With the wrapper:

```bash
ntm 1 view note
```

File `1` is used only for that execution, keeping the current context unchanged.

---

### Interactive search with `fview` (fzf + bat)

The `fview` function allows interactive searching within note bodies using `fzf`.

Basic example:

```bash
ntm view note -b
```

For long texts:

```bash
ntm view note -b | fview
```

An `fzf` session starts:

1. Type the word or phrase to search
2. Select the desired line
3. Press `Enter`

The output reopens in standard `bat` view, positioned on the selected line.

---

> ⚠️ The Zsh wrapper is completely optional.
> NotaMy works correctly without it.

---

## ✅ FINAL NOTES

* Tags and hashes are case-insensitive.
* Abbreviations are allowed if unambiguous.
* Running `ntm_nc` starts the program in no-color mode (no ANSI escape sequences).

---

## 📁 CONFIGURATION

NotaMy behavior can be customized via the configuration file:

```
~/.config/ntm.conf
```

You can configure:

* Interface colors
* Default editor
* Default file paths

---
