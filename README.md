> this is a work in progress, but it may not actually happen.

### Project Layout

- LetDB: database server, core engine
- LetDD: database driver, utilizing `LetDB` under the hood

### Status

this project is still under active development

### Goals

- reliable account and transfer handling
- powerful storage with write-ahead logging
- small codebase, without any dependencies in the core engine
- prioritize durability over performance
- frequent use of the latest c features is deliberate

### Notes

- public exposure should happen through `LetDD`, not `LetDB`
- licensing might change
