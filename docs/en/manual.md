
---

# 📒 `NotaMy` Documentation

`NotaMy` (`ntm`) is a **command-line tool** for the **hierarchical management of notes, files, and personal information**. It organizes your entries using a **tree structure** with **tags**, **keywords**, and **unique hashes**, making it easy to add, edit, search, and view data.

> Perfect for terminal-based users looking for a fast, flexible, and structured way to store and access information.

---

## 📌 SYNOPSIS

```bash
ntm [COMMAND] [SUBCOMMAND] [OPTIONS]  # Full command with subcommand
ntm [COMMAND] [OPTIONS]               # Simple command (e.g., find, help)
```

---

## 🧭 AVAILABLE COMMANDS

| Command    | Subcommand | Description                                                  |
| ---------- | ---------- | ------------------------------------------------------------ |
| `add`      | `note`     | Adds a new note                                              |
|            | `file`     | Adds a new archive note file                                 |
| `view`     | `note`     | Displays saved notes                                         |
|            | `file`     | Lists available note files                                   |
|            | `tag`      | Shows the tag hierarchy                                      |
| `find`     |            | Searches notes by criteria (tag, hash, keywords, date, etc.) |
| `modify`   |            | Modifies an existing note                                    |
| `organize` |            | Moves a note within the hierarchy                            |
| `remove`   |            | Deletes a note                                               |
| `setting`  |            | Sets a note file as active                                   |
| `editor`   |            | Sets the text editor (`vim`, `nano`, `nul`)                  |
| `backup`   |            | Creates a backup of the active note file                     |
| `help`     |            | Displays general help                                        |

---

## ⚙️ AVAILABLE OPTIONS

> Options vary by command. Common ones are listed below:

### `add note`

| Short Option | Long Option    | Description                                    |
| ------------ | -------------- | ---------------------------------------------- |
| `-t`         | `--tag`        | Adds a tag (required, default: `"?"`)          |
| `-c`         | `--comment`    | Adds a comment to the note (required)          |
| `-k`         | `--keywords`   | Adds one or more space-separated keywords      |
| `-f`         | `--file`       | Attaches a file                                |
| `-b`         | `--body`       | Opens the editor to write the note body        |
| `-i`         | `--input`      | Disables the editor; allows input from `stdin` |
| `-p`         | `--protection` | Enables encryption for the note                |

### `add file`

| Required Argument | Options        |
| ----------------- | -------------- |
| `<path/to/file>`  | `-c` (comment) |

### `view note`, `view tag`, `view file`

| Short Option | Long Option    | Description                          |
| ------------ | -------------- | ------------------------------------ |
| `-e`         | `--extended`   | Extended view (hash, date)           |
| `-b`         | `--body`       | Shows the body of notes              |
| `-p`         | `--protection` | Includes protected (encrypted) notes |

### `find`

| Option           | Description                                      |
| ---------------- | ------------------------------------------------ |
| `-t`             | Search by tag                                    |
| `-h`             | Search by specific hash                          |
| `-k`             | Search by keywords                               |
| `-d`             | Search by date (relative or absolute)            |
| `-f`             | Opens the attached file with the selected editor |
| `-o`             | Prints the attached file instead of opening it   |
| `-e`, `-b`, `-p` | Same as in `view` commands                       |

### `modify`

| Required Argument | Same options as `add note` |
| ----------------- | -------------------------- |
| `<hash>`          |                            |

### `organize`

| Required Argument | Destination                                 |
| ----------------- | ------------------------------------------- |
| `<note hash>`     | `"."` (root), `up`, `down`, or another hash |

### `remove`

| Required Argument | Description            |
| ----------------- | ---------------------- |
| `-t`, `-h`        | Removes by tag or hash |

---

## ✍️ ADDING NOTES

### Basic Example

```bash
$ ntm add note -t "tag1" -c "comment" -k "key1 key2" -f "/path/file.txt" -b
```

> The selected editor will launch to write the note body.

### Attaching a File

* Use `-f` followed by the full path or shorthand with `$HOME` or `$(pwd)`:

```bash
$ ntm add note -t "docs" -c "report" -f "$HOME/reports/report.txt"
```

### Adding Keywords

* Use `-k` followed by one or more space-separated keywords:

```bash
$ ntm add note -t "project" -c "description" -k "java API spring"
```

> Keywords improve future searchability and show in `-be` mode.

### Using Variables or Dynamic Commands

Use environment variables or shell commands to add dynamic info:

```bash
$ ntm add note -t "log" -c "logged on $(date)"
```

### Redirecting Output to Note Body

You can pipe output directly into a note:

```bash
$ ls | ntm add note -t "directory" -c "current content" -bi
```

> `-i` is needed to read from stdin.

### Direct Note Nesting

Use < or > in tag names to create nested note structures:

```bash
$ ntm add note -t "projects>API" -c "Note inside API"
```

or

```bash
$ ntm add note -t "API<projects" -c "Note inside API"
```

> Both formats are valid.

### Note Protection

* Use `-p` to encrypt the note:

```bash
$ ntm add note -t "security" -c "encrypted content" -p
```

---

## 🔎 VIEWING & SEARCHING

NotaMy offers powerful search and an intuitive display.

### Tag Overview

To see the tag hierarchy:

```bash
$ ntm view tag
$ ntm view tag -e   # Shows hashes too
```

### Note Listing

Use `view note` to list all notes:

```bash
$ ntm view note
```

### Search by Tag, Hash, Keywords, or Date

```bash
$ ntm find -t "projects"
$ ntm find -h "a1b2"
$ ntm find -k "API spring"
```

> Customize view with `-e`, `-b`, `-p`.

### Time-based Search

* **Relative** (use `Y`, `M`, `D`, `h`, `m`, `s`):

```bash
$ ntm find -d 2D    # Last 2 days
```

* **Absolute** (date ranges or specific months):

```bash
$ ntm find -d "2024-05"
$ ntm find -d "2024-01 to 2024-03"
```

### Expand/Collapse Hierarchy

```bash
$ ntm find -t "tag1+"    # Expand to children/subchildren
$ ntm find -t "tag1-"    # Show siblings only
```

> Search is **case-insensitive** and **partial** (prefix match if unique).

---

## 📂 ATTACHED FILE MANAGEMENT

### Open or Print Attached Files

```bash
$ ntm find -ft "tag1"   # Opens file with editor
$ ntm find -fot "tag1"  # Prints file to terminal
```

> `-o` is required to output to stdout.

---

## ✏️ NOTE STRUCTURE & EDITING

### Modify a Note

```bash
$ ntm modify "5b43" -t "new_tag" -c "new comment"
$ ntm modify "5b43" -b   # Edit body
```

> If editor is `nul`, text is overwritten; with `vim/nano`, it's in-place.

### Reorganize Notes

```bash
$ ntm organize "5b43" "ff46"   # Move 5b43 into ff46
$ ntm organize "5b43" .        # Move to root
$ ntm organize "5b43" up       # Move among siblings
```

### Remove a Note

```bash
$ ntm remove -h "5b43"
```

⚠️ **Warning:** Deletion is recursive — descendants will also be deleted!

---

## ⚙️ SETTINGS

### Set Editor

You can choose: `nul`, `vim`, or `nano`.

```bash
$ ntm editor vim
```

### Add Archive File

Add a new file with the `add file` command:

```bash
$ ntm add file "$HOME/path/new_notes.X" -c "2024 Notes"
```

### View Available Files

```bash
$ ntm view file
```

### Set Active File

```bash
$ ntm setting 0
```

> The index is shown with `view file`.

---

## 🛡️ BACKUP

```bash
$ ntm backup
```

Creates a backup of the active note file.

---

## ❓ HELP

```bash
$ ntm help
```

---

## ✅ FINAL NOTES

* Tags and hashes are case-insensitive
* You can use abbreviations as long as they are unambiguous
* Running the program as `ntm_nc` enables **no color** mode

---

## 📁 CONFIGURATION

You can customize the program via:

```
~/.config/ntm.conf
```

In this file, you can configure:

* Text UI color schemes
* Default text editor
* Default file paths

---

