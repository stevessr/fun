use itertools::Itertools;

fn solve() {
    let mut input = String::new();
    std::io::stdin().read_line(&mut input).unwrap();
    let mut iter = input.split_whitespace();
    let n: usize = iter.next().unwrap().parse().unwrap();
    let k: usize = iter.next().unwrap().parse().unwrap();

    input.clear();
    std::io::stdin().read_line(&mut input).unwrap();
    let s = input.trim().to_string();
    let mut chars: Vec<char> = s.chars().collect();
    let mut rev_chars = chars.clone();
    rev_chars.reverse();

    let mut diff_count = 0;
    for i in 0..n {
        if chars[i] != rev_chars[i] {
            diff_count += 1;
        }
    }
    diff_count /= 2;

    if s < rev_chars.iter().collect::<String>() {
        println!("YES");
        return;
    }

    if k >= diff_count {
        println!("YES");
    } else {
        println!("NO");
    }
}

fn main() {
    let mut input = String::new();
    std::io::stdin().read_line(&mut input).unwrap();
    let t: usize = input.trim().parse().unwrap();
    for _ in 0..t {
        solve();
    }
}