use std::io::BufRead;
use std::time;
use std::time::{Instant};

pub struct Shape {
    pub size: usize,
    pub fields: Vec<(usize, usize)>,
    pub rightmost_field: usize,
    pub bottommost_field: usize
}

pub struct Grid {
    pub size: usize,
    pub fields: Vec<Vec<Option<i32>>>
}

pub struct Environment {
    pub island_size: usize,
    pub num_of_shapes: usize,
    pub epsilon: f64,
    pub shapes: Vec<Shape>,
    pub grid: Grid,
    pub time_start: time::Instant
}

impl Shape {
    pub fn new(size: usize, fields: Vec<(usize, usize)>) -> Self {
        let mut rightmost_field = 0;
        let mut bottommost_field = 0;
        for (x, y) in &fields {
            if *y > rightmost_field {
                rightmost_field = *y;
            }
            if *x > bottommost_field {
                bottommost_field = *x;
            }
        }
        Shape { size, fields, rightmost_field, bottommost_field }
    }

    pub fn shifted(&self, horizontal_shift: usize, vertical_shift: usize) -> Vec<(usize, usize)> {
        let mut shifted = Vec::new();
        for (x, y) in &self.fields {
            shifted.push((x + vertical_shift, y + horizontal_shift));
        }
        shifted
    }

    pub fn horizontal_projection(&self, shift: usize) -> Vec<i32> {
        let mut projection = Vec::new();

        for (_x, y) in &self.fields {
            if projection.len() <= *y {
                projection.resize(*y + 1, 0);
            }
            projection[*y + shift] += 1;
        }
        projection
    }

    pub fn vertical_projection(&self, shift: usize) -> Vec<i32> {
        let mut projection = Vec::new();

        for (x, _y) in &self.fields {
            if projection.len() <= *x {
                projection.resize(*x + 1, 0);
            }
            projection[*x + shift] += 1;
        }
        projection
    }
}

fn read_environment() -> Environment {
    let mut env = Environment {
        island_size: 0,
        num_of_shapes: 0,
        epsilon: 0.0,
        shapes: Vec::new(),
        grid: Grid { size: 0, fields: Vec::new() },
        time_start: time::Instant::now()
    };

    let stdin = std::io::stdin();
    let mut lines = stdin.lock().lines();
    let line = lines.next().unwrap().unwrap();
    let mut parts = line.split_whitespace();
    env.island_size = parts.next().unwrap().parse().unwrap();
    env.num_of_shapes = parts.next().unwrap().parse().unwrap();
    env.epsilon = parts.next().unwrap().parse().unwrap();

    env.grid.size = env.island_size;
    for _ in 0..env.island_size {
        let mut row = Vec::new();
        for _ in 0..env.island_size {
            row.push(None);
        }
        env.grid.fields.push(row);
    }

    for _ in 0..env.num_of_shapes {
        let line = lines.next().unwrap().unwrap();
        let mut parts = line.split_whitespace();
        let size = parts.next().unwrap().parse().unwrap();
        let mut fields = Vec::new();
        for _ in 0..size {
            let x = parts.next().unwrap().parse().unwrap();
            let y = parts.next().unwrap().parse().unwrap();
            fields.push((x, y));
        }
        env.shapes.push(Shape::new(size, fields));
    }

    env
}

impl Environment {

    pub fn query_without_update(&self, x: usize, y: usize) -> i32 {
        println!("q 1 {} {} ", x, y);
        let stdin = std::io::stdin();
        let mut lines = stdin.lock().lines();
        let line = lines.next().unwrap().unwrap();
        println!("# result: {}", line);
        let mut parts = line.split_whitespace();
        let result = parts.next().unwrap().parse().unwrap();
        result
    }

    pub fn query(&mut self, amount: usize, queries: &Vec<(usize, usize)>) -> i32 {
        print!("q {} ", amount);
        for (x, y) in queries {
            print!("{} {} ", x, y);
        }
        println!();

        let stdin = std::io::stdin();
        let mut lines = stdin.lock().lines();
        let line = lines.next().unwrap().unwrap();
        println!("# result: {}", line);
        let mut parts = line.split_whitespace();
        let result = parts.next().unwrap().parse().unwrap();

        if amount == 1 {
            self.grid.fields[queries[0].0][queries[0].1] = Some(result);
        }
        result
    }

    pub fn answer(&self, amount: usize, answers: Vec<(usize, usize)>) -> i32 {

        print!("a {} ", amount);
        for (x, y) in answers {
            print!("{} {} ", x, y);
        }
        println!();

        let stdin = std::io::stdin();
        let mut lines = stdin.lock().lines();
        let line = lines.next().unwrap().unwrap();
        println!("# result: {}", line);
        let mut parts = line.split_whitespace();
        let result = parts.next().unwrap().parse().unwrap();

        if result == 1 {
            std::process::exit(0);
        }
        result
    }

    pub fn check_time_limit(&self) {
        let now = Instant::now();
        if now.duration_since(self.time_start).as_millis() > 1500 {
            println!("# time limit exceeded");
            brute_force(&self);
            std::process::exit(0);
        }
    }


    pub fn get_max_horizontal_shift(&self, shape: &Shape) -> i32 {
        self.island_size as i32 - shape.rightmost_field as i32
    }

    pub fn get_max_vertical_shift(&self, shape: &Shape) -> i32 {
        self.island_size as i32 - shape.bottommost_field as i32
    }

}

fn brute_force(env: &Environment) {
    let mut places = Vec::new();
    for x in 0..env.island_size {
        for y in 0..env.island_size {
            if env.grid.fields[x][y].is_none() {
                let w = env.query_without_update(x, y);
                if w > 0 {
                    places.push((x, y));
                }
            }
            else if env.grid.fields[x][y].unwrap() > 0 {
                places.push((x, y));
            }
        }
    }
    assert_eq!(1, env.answer(places.len(), places));
}

pub struct Guess {
    pub env: *mut Environment,
    pub horizontal_guess: Vec<f64>,
    pub vertical_guess: Vec<f64>,
    pub guess_repeat_config: usize,
}

impl Guess {

    pub fn new(env: &mut Environment, guess_repeat_config: usize) -> Self {
        let mut horizontal_guess = Vec::new();
        let mut vertical_guess = Vec::new();
        for _ in 0..env.island_size {
            horizontal_guess.push(0.0);
            vertical_guess.push(0.0);
        }
        Guess { env, horizontal_guess, vertical_guess, guess_repeat_config }
    }

    pub fn guess_range(&self, _k: usize) -> f64 {
        3.0
    }

    pub unsafe fn set_guess_range_with_queries(&mut self) {
        let env = &mut *self.env;
        let _k = env.island_size;

        for i in 0..env.island_size {
            let mut row = Vec::new();
            for j in 0..env.island_size {
                row.push((i, j));
            }

            let mut sum = 0;
            for _ in 0..self.guess_repeat_config {
                sum += env.query(row.len(), &row);
            }
            let avg = sum as f64 / self.guess_repeat_config as f64;
            self.vertical_guess[i] = avg;
        }

        for i in 0..env.island_size {
            let mut col = Vec::new();
            for j in 0..env.island_size {
                col.push((j, i));
            }
            let mut sum = 0;
            for _ in 0..self.guess_repeat_config {
                sum += env.query(col.len(), &col);
            }
            let avg = sum as f64 / self.guess_repeat_config as f64;
            self.horizontal_guess[i] = avg;
        }
    }

}

pub struct State {
    pub env: *const Environment,
    pub horizontal_shifts: Vec<usize>,
    pub vertical_shifts: Vec<usize>,
    pub grid: Vec<Vec<i32>>,
}

impl State {

    pub fn new(env: &Environment, horizontal_shifts: Vec<usize>, vertical_shifts: Vec<usize>) -> Self {
        let mut grid = vec![vec![0; env.island_size]; env.island_size];
        for i in 0..env.num_of_shapes {
            let shape = &env.shapes[i];
            for j in 0..shape.size {
                grid[shape.fields[j].0 + vertical_shifts[i]][shape.fields[j].1 + horizontal_shifts[i]] += 1;
            }
        }
        State { env, horizontal_shifts, vertical_shifts, grid }
    }

    pub fn is_valid(&self) -> bool {
        let env = unsafe { &*self.env };
        for x in 0..env.island_size {
            for y in 0..env.island_size {
                if env.grid.fields[x][y].is_some() && env.grid.fields[x][y].unwrap() != self.grid[x][y] {
                    return false;
                }
            }
        }
        true
    }

    pub fn try_answer(&self) {
        let env = unsafe { &*self.env };
        let mut places = Vec::new();
        for x in 0..env.island_size {
            for y in 0..env.island_size {
                if self.grid[x][y] > 0 {
                    places.push((x, y));
                }
            }
        }
        env.answer(places.len(), places);
    }


}

pub struct PlacementManager {
    pub env: *const Environment,
    pub guess: Vec<f64>,
    pub guess_range: f64,
    pub projections: Vec<Vec<i32>>,
    pub max_shifts: Vec<i32>,
    pub shifts: Vec<Vec<usize>>
}

impl PlacementManager {

    pub fn new(env: &Environment, guess: Vec<f64>, guess_range: f64, projections: Vec<Vec<i32>>, max_shifts: Vec<i32>) -> Self {
        PlacementManager { env, guess, guess_range, projections, max_shifts, shifts: Vec::new() }
    }

    pub fn shift(&self, projection: &Vec<i32>, shift: usize) -> Vec<i32> {
        let mut new_projection = Vec::new();
        for _ in 0..shift {
            new_projection.push(0);
        }
        for i in 0..projection.len() {
            new_projection.push(projection[i]);
        }
        new_projection
    }

    pub fn is_valid(&self, placement: &Vec<usize>, check_lower_bound: bool) -> bool {
        let env = unsafe { &*self.env };
        let mut filled = vec![0; env.island_size];
        for i in 0..placement.len() {
            let projection = self.shift(&self.projections[i], placement[i]);
            for j in 0..projection.len() {
                filled[j] += projection[j];
            }
        }

        for i in 0..filled.len() {
            if (check_lower_bound && filled[i] < (self.guess[i] - self.guess_range) as i32) || filled[i] > (self.guess[i] + self.guess_range) as i32 {
                return false;
            }
        }

        true
    }

    pub fn backtracking(&mut self, placement: Vec<usize>) {
        let env = unsafe { &*self.env };
        env.check_time_limit();
        if !self.is_valid(&placement, false) {
            return;
        }
        let index = placement.len();
        if index == env.num_of_shapes {
            if self.is_valid(&placement, true) {
                self.shifts.push(placement.clone());
            }
        }
        else {
            for i in 0..self.max_shifts[index] {
                env.check_time_limit();
                let mut new_placement = placement.clone();
                new_placement.push(i as usize);
                self.backtracking(new_placement);
            }
        }
    }

}

pub struct Solver {
    pub env: *mut Environment,
    pub horizontal_placement_manager: PlacementManager,
    pub vertical_placement_manager: PlacementManager,
    pub horizontal_guess: Vec<f64>,
    pub vertical_guess: Vec<f64>,
    pub horizontal_projections: Vec<Vec<i32>>,
    pub vertical_projections: Vec<Vec<i32>>,
}

impl Solver {
    pub fn new(env: &mut Environment, guess: &Guess, guess_range: f64) -> Self {
        let mut horizontal_projections = Vec::new();
        let mut vertical_projections = Vec::new();

        for i in 0..env.num_of_shapes {
            horizontal_projections.push(env.shapes[i].horizontal_projection(0));
            vertical_projections.push(env.shapes[i].vertical_projection(0));
        }

        let mut max_horizontal_shifts = Vec::new();
        let mut max_vertical_shifts = Vec::new();

        for i in 0..env.num_of_shapes {
            max_horizontal_shifts.push(env.get_max_horizontal_shift(&env.shapes[i]));
            max_vertical_shifts.push(env.get_max_vertical_shift(&env.shapes[i]));
        }

        let horizontal_placement_manager = PlacementManager::new(
            &env,
            guess.horizontal_guess.clone(),
            guess_range,
            horizontal_projections.clone(),
            max_horizontal_shifts
        );
        let vertical_placement_manager = PlacementManager::new(
            &env,
            guess.vertical_guess.clone(),
            guess_range,
            vertical_projections.clone(),
            max_vertical_shifts
        );

        Solver {
            env,
            horizontal_placement_manager,
            vertical_placement_manager,
            horizontal_guess: guess.horizontal_guess.clone(),
            vertical_guess: guess.vertical_guess.clone(),
            horizontal_projections,
            vertical_projections
        }
    }

    pub fn query_random(&self) -> i32 {
        let env = unsafe { &mut *self.env };
        let x = rand::random::<usize>() % env.island_size;
        let y = rand::random::<usize>() % env.island_size;
        if env.grid.fields[x][y].is_some() {
            return env.grid.fields[x][y].unwrap();
        }
        else { return env.query(1, &vec![(x, y)]); }
    }

    pub fn get_valid_trials(&self) -> Vec<State> {
        let env = unsafe { &*self.env };
        let mut valid_trials = Vec::new();
        for horizontal_shifts in &self.horizontal_placement_manager.shifts {
            for vertical_shifts in &self.vertical_placement_manager.shifts {
                let state = State::new(env, horizontal_shifts.clone(), vertical_shifts.clone());
                if state.is_valid() {
                    valid_trials.push(state);
                }
            }
        }
        valid_trials
    }

    pub fn solve(&mut self) {
        let env = unsafe { &*self.env };
        println!("# horizontal backtracking:");
        self.horizontal_placement_manager.backtracking(Vec::new());
        println!("# vertical backtracking:");
        self.vertical_placement_manager.backtracking(Vec::new());

        let expected_trials = self.horizontal_placement_manager.shifts.len() * self.vertical_placement_manager.shifts.len();
        println!("# expected trials: {}", expected_trials);
        if expected_trials > 100000 {
            for _ in 0..20 {
                self.query_random();
            }
            return;
        }
        let mut valid_trials = self.get_valid_trials();
        while valid_trials.len() > 20 {
            env.check_time_limit();
            self.query_random();
            valid_trials = self.get_valid_trials();
        }

        for trial in valid_trials {
            trial.try_answer();
        }

        println!("# could not find a solution");

    }
}

fn main() {
    let mut env = read_environment();
    env.check_time_limit();

    if env.num_of_shapes > 5 && env.epsilon > 0.03 {
        brute_force(&env);
    }

    let repeat_config = 2;
    let mut guess = Guess::new(&mut env, repeat_config);
    unsafe { guess.set_guess_range_with_queries(); }

    println!("# horizontal_guess: {:?}", guess.horizontal_guess);
    println!("# vertical_guess: {:?}", guess.vertical_guess);

    let mut guess_range = 0.2;
    let mut solver = Solver::new(&mut env, &guess, guess_range);
    for _ in 0..20 {
        solver.query_random();
    }
    loop {
        env.check_time_limit();
        solver.solve();
        guess_range += 1.0;
        solver = Solver::new(&mut env, &guess, guess_range);
    }

}
