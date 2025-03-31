# erm::ArgsParser

### Table of contents

- [erm::ArgsParser](#ermargsparser)
    - [Table of contents](#table-of-contents)
  - [Intro](#intro)
  - [SubCommands](#subcommands)
  - [OptionalArgs](#optionalargs)
  - [HelpArg](#helparg)
  - [PositionalArgs](#positionalargs)
  - [ArgValues](#argvalues)

---

## Intro

`erm::ArgsParser` provide a set of utilities to develop CLI applications. In particular, it helps parsing arguments passed to the executable invocation.

Arguments are divided into three different categories, respectively: [SubCommands](#subcommands), [OptionalArgs](#optionalargs) and [PositionalArgs](#positionalargs).

By default `erm::ArgsParser` owns an `erm::SubCommand` instance which is a "special" instance in the sense that, contrary to the user created ones, it will be named after the executable. This `erm::SubCommand` is the main entry point for the user to configure the parser and it can be accessed through the `erm::ArgsParser::operator->` operator overload.

After configuring the parser, the user can call `erm::ArgsParser::parseArgs`, the method will try to parse the args list based on the parser configuration and in case of failure (or presence of the `help` argument) it will print a description of what (eventually) went wrong and the help text subsequently and return `nullptr`, otherwise it will return a pointer to the `erm::SubCommand` that matched the arguments' list.

## SubCommands

`erm::SubCommand` is the main component of the parser, that's where the parsing logic is, as well as informations about all the accepted arguments and sub-commands.

From a user perspective, a sub-command corresponds to a sub-set of actions that can be accessed by entering the sub-command name, i.e. let's assume you are developing a CLI text editor called `ted` which allows users: **1.** to invoke it without arguments to open an empty editor or **2.** with a sub-command to access a specific funcionality:
```bash
ted                         # Opens an empty editor
ted -f "<filename>"            # Opens the editor loading the file <filename>
ted format "<filename>"     # Access the `format` sub-command which applies some kind of formatting to the file <filename>
```
In short, sub-commands allows the user to specify a completely new set of options and arguments available for a different use case of the executable invocation.

## OptionalArgs

## HelpArg

## PositionalArgs

## ArgValues