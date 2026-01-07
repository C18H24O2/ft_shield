# [ft_shield](https://projects.intra.42.fr/42cursus-ft_shield/kiroussa) (& [matt-daemon](https://projects.intra.42.fr/42cursus-matt-daemon/kiroussa))

[This repository](https://codeberg.org/27/ft_shield) contains the source code for both the `ft_shield` and `matt-daemon` projects.

To build either project, you can:
- Clone the repository from the right url ([27/ft_shield](https://codeberg.org/27/ft_shield) or [27/matt-daemon](https://codeberg.org/27/matt-daemon))
  - It will determine automagically which project you want to build based on the remote url of your git repository.
- If you want to remove the git remote url, you can rename the repository to `ft_shield` or `matt-daemon` and it will determine the type that way.
- In an already cloned repo, run `make matt-daemon` or `make shield`
  - Note that trying to make the executables (`ft_shield` / `MattDaemon`) directly **will** fail, use the wrapper tasks instead.

## To 42 staff

If this triggered some kind of anti-cheating measure, hope we made you laugh for a sec.

## License

This project is licensed under the [ISC License](./LICENSE).
