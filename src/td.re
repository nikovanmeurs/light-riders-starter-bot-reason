type direction = [ | `Up | `Down | `Left | `Right | `Stop];

type cell = [ | `Empty | `Wall | `Player0 | `Player1];

type game_info = {
  mutable timebank: int,
  mutable time_per_move: int,
  mutable player_names: list string,
  mutable your_bot: string,
  mutable your_botid: int,
  mutable width: int,
  mutable height: int,
  mutable max_rounds: int
};

type location = {mutable row: int, mutable col: int};

type player = {id: int, loc: location, prev_loc: location, mutable dir: direction};

type game_state = {
  mutable field: array (array cell),
  mutable round: int,
  mutable last_update: float,
  mutable last_timebank: int,
  mutable player: array player,
  setup: game_info
};
