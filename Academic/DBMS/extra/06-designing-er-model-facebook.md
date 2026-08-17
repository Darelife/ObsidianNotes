# Lecture 6: Designing ER Model of Facebook

This lecture applies the methodology from Lecture 5 to a real, large-scale system: a simplified Facebook-like social media platform. It's a case study in handling self-referencing relationships, weak entities, and many-to-many associations together.

## 1. Requirement Gathering (Informal Problem Statement)

A social media platform needs to support:
- Users who have profile information.
- Users can befriend other users (a symmetric, self-referencing relationship).
- Users can create posts, which contain text/images.
- Users can comment on posts.
- Users can like posts and comments.
- Users can create/join groups.
- Users can send messages to each other.

## 2. Identifying Entities

| Entity | Key Attributes |
|---|---|
| `User` | user_id (PK), name, email, dob, gender |
| `Post` | post_id (PK), content, timestamp, privacy_setting |
| `Comment` | comment_id (PK — weak, depends on Post), text, timestamp |
| `Group` | group_id (PK), group_name, description |
| `Message` | message_id (PK), content, timestamp |

`User` has composite attribute `name` (first_name, last_name) and a multi-valued attribute if we allow multiple `phone_numbers`.

## 3. Identifying Relationships

### a. Friendship — Self-Referencing (Unary/Recursive) Relationship, M:N
Users befriend other users. This is a **recursive relationship** — `User` relates to itself via a `Friends_With` relationship set.
- Cardinality: M:N (a user can have many friends, and can be a friend to many others).
- Since friendship is symmetric, this is often modeled as a single unordered relationship rather than two directional roles.

### b. Posts — 1:N between User and Post
- One `User` **creates** many `Post`s; each `Post` belongs to exactly one `User`.
- Total participation on the `Post` side (every post must have a creator); partial on the `User` side (not every user has posted).

### c. Comments — Weak Entity Pattern
- `Comment` is a **weak entity** — it cannot exist without a `Post` (its owner/strong entity).
- Relationship `Has_Comment` between `Post` and `Comment` is the **identifying relationship**, drawn with a double diamond; `Comment` is drawn in a double rectangle.
- Additionally, a `Comment` is **written by** a `User` → 1:N relationship (one user writes many comments).

### d. Likes — Many-to-Many with Attributes (needs its own resolution)
- A `User` can like many `Post`s, and a `Post` can be liked by many `User`s → M:N relationship `Likes`.
- Since `Likes` may need its own attribute (e.g., `timestamp` of the like, or `reaction_type` — like/love/haha), this M:N relationship carries attributes, drawn attached directly to the `Likes` diamond.
- Similarly, users can like `Comment`s too — this could be modeled as a separate `Likes_Comment` relationship, or by generalizing `Post` and `Comment` into a common `Content` superclass (see Extended ER, Lecture 4) so a single `Likes` relationship handles both.

### e. Group Membership — M:N with Attributes
- `User` **joins** `Group` → M:N relationship `Member_Of`, with attributes like `joined_date`, `role` (admin/member).

### f. Messaging — Self-Referencing M:N with Attributes
- `User` **sends** `Message` **to** `User` → this is modeled either as a ternary-like structure or, more cleanly, `Message` is its own entity with two 1:N relationships: `Sent_By` (User → Message) and `Received_By` (User → Message).

## 4. Applying Extended ER Concepts

- **Generalization**: `Post` and `Comment` (and even `Message`) share common traits — text content, timestamp, author, likeable — these could be generalized into a `Content` superclass, with `Post`, `Comment` as (overlapping or disjoint, total) subclasses. This reduces redundant relationship design for `Likes`.
- **Aggregation**: If we wanted to track "who moderated/flagged a specific like event," we'd aggregate the `Likes` relationship (User–Post) into a single unit and connect it to a `Moderator` via a `Flags` relationship.

## 5. Simplified Diagram Description

```
User ──(Friends_With, M:N, recursive)── User
User ──(Creates, 1:N)── Post ──(Has_Comment, identifying)── Comment
User ──(Writes, 1:N)── Comment
User ──(Likes, M:N, attr: reaction_type)── Post
User ──(Member_Of, M:N, attr: role, joined_date)── Group
User ──(Sent_By, 1:N)── Message ──(Received_By, N:1)── User
```

## 6. Key Design Decisions Worth Noting

1. **Recursive relationships** (Friends_With, Message sender/receiver) are common in social platforms and require special care during relational mapping — they typically result in a foreign key referencing the *same* table.
2. **Weak entities** (`Comment`) exist because a comment has no independent meaning without its parent post.
3. **M:N relationships with attributes** (`Likes`, `Member_Of`) will each become their own junction/bridge table in the relational model (Lecture 8), since a plain foreign key can't capture M:N cardinality.

## Summary
Designing Facebook's ER model showcases three advanced patterns beyond basic entity-relationship pairs: a **recursive M:N relationship** for friendships, a **weak entity** for comments dependent on posts, and **M:N relationships carrying their own attributes** for likes and group memberships. This case study is a bridge between the pure theory of Lectures 3–5 and the practical translation into relational tables covered next.

## 7. Additional Consideration: Privacy and Access Control Modeling

Real systems like Facebook also model **privacy settings** per post (public/friends-only/custom) — this could be an attribute on `Post`, or, for finer-grained control, a separate `Post_Visibility(post_id, user_id)` table listing exactly which users can view a custom-shared post — another example of a design decision driven by requirements, not just raw entity/relationship identification.

## 8. Alternative Designs Worth Discussing in Interviews

- Modeling `Friends_With` as a **directed "Friend Request"** relationship (pending/accepted/rejected status) is more realistic than a simple symmetric M:N relationship, and is a common interview follow-up ("how would you model friend requests, not just confirmed friendships?"). This typically requires a `Friend_Request(sender_id, receiver_id, status, timestamp)` table.
- Modeling `Notifications` as another entity generated by various actions (likes, comments, friend requests) is a common extension interviewers ask about.

---

## Placement Interview Questions — Lecture 6

**Q1. How would you model the "friendship" relationship between users in an ER diagram?**
As a recursive (self-referencing/unary) many-to-many relationship on the `User` entity, since a user can be friends with many other users, and friendship is symmetric.

**Q2. Why is "Comment" modeled as a weak entity in the Facebook ER design?**
Because a comment has no independent existence or unique identity without being tied to a specific post — it depends on `Post` as its owning entity, and its identifying relationship (`Has_Comment`) ties it to that post.

**Q3. How would you handle "Likes" on both Posts and Comments without duplicating relationship logic?**
By generalizing `Post` and `Comment` into a common `Content` superclass (Extended ER — generalization) and defining a single `Likes` relationship between `User` and `Content`, rather than two separate Likes relationships.

**Q4. How do you model a self-referencing relationship like "User sends Message to User" in the relational model?**
`Message` becomes its own entity/table, with two separate foreign keys both referencing `User.user_id` — e.g., `sender_id` and `receiver_id` — rather than trying to force a single FK to represent both roles.

**Q5. Why does the "Likes" relationship need to be modeled with its own attributes rather than just a simple M:N link?**
Because we often need extra information about the like itself (e.g., `reaction_type`, `timestamp`), which can't be attached to either User or Post alone — it belongs specifically to the *relationship instance* between a particular user and a particular post.

**Q6. How would you extend this design to support friend requests instead of just confirmed friendships?**
Replace the plain symmetric `Friends_With` M:N relationship with a directed relationship/entity `Friend_Request(sender_id, receiver_id, status, timestamp)`, where `status` can be pending/accepted/rejected, allowing asymmetric intermediate states before a friendship is confirmed.
