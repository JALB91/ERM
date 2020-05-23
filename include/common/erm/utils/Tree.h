#pragma once

#include <functional>
#include <memory>
#include <vector>

namespace erm {

	template<typename S, typename T>
	class Tree
	{
	public:
		typedef std::vector<std::unique_ptr<Tree>> Children;

	public:
		Tree(S id, T payload)
			: mParent(nullptr)
			, mId(std::forward<S>(id))
			, mPayload(std::forward<T>(payload))
		{}
		~Tree() = default;

		Tree(Tree&& other)
			: mParent(std::move(other.mParent))
			, mChildren(std::move(other.mChildren))
			, mId(std::move(other.mId))
			, mPayload(std::move(other.mPayload))
		{
			for (auto& child : mChildren)
			{
				child->mParent = this;
			}
		}

		Tree& operator=(Tree&& other)
		{
			if (&other == this)
				return *this;

			mParent = std::move(other.mParent);
			mChildren = std::move(other.mChildren);
			mId = std::move(other.mId);
			mPayload = std::move(other.mPayload);

			for (auto& child : mChildren)
			{
				child->mParent = this;
			}

			return *this;
		}

		Tree& GetRoot()
		{
			Tree* parent = mParent;
			while (parent)
			{
				if (parent->mParent)
				{
					parent = parent->mParent;
				}
				else
				{
					return *parent;
				}
			}
			return *this;
		}

		Tree& AddChild(S id, T payload)
		{
			if (Tree* child = Tree::Find(GetRoot(), id))
			{
				child->SetPayload(std::forward<T>(payload));
				return *child;
			}
			auto& child = mChildren.emplace_back(std::make_unique<Tree>(std::forward<S>(id), std::forward<T>(payload)));
			child->mParent = this;
			return *child;
		}

		inline void ForEachDo(const std::function<void(Tree&)>& before, const std::function<void(Tree&)>& after = nullptr)
		{
			ForEachDo(*this, before, after);
		}

		inline void Find(S id)
		{
			return Find(*this, id);
		}

		inline unsigned int GetSize()
		{
			unsigned int size = 0;
			ForEachDo([&size](Tree& /*node*/) {
				++size;
			});
			return size;
		}

		inline void SetPayload(T payload) { mPayload = std::forward<T>(payload); }
		inline const T& GetPayload() const { return mPayload; }
		inline T& GetPayload() { return mPayload; }
		inline S GetId() const { return mId; }
		inline Tree* GetParent() { return mParent; }
		inline const Children& GetChildren() const { return mChildren; }

	public:
		static void ForEachDo(Tree& node, const std::function<void(Tree&)>& before, const std::function<void(Tree&)>& after)
		{
			if (before)
				before(node);
			for (auto& child : node.mChildren)
			{
				ForEachDo(*child, before, after);
			}
			if (after)
				after(node);
		}

		static Tree* Find(Tree& node, S id)
		{
			if (node.mId == id)
				return &node;

			for (auto& child : node.mChildren)
			{
				if (Tree* result = Find(*child, id))
				{
					return result;
				}
			}

			return nullptr;
		}

	private:
		Tree* mParent;
		Children mChildren;
		S mId;
		T mPayload;
	};

} // namespace erm
