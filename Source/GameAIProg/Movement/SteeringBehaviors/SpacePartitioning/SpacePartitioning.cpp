#include "SpacePartitioning.h"

// --- Cell ---
// ------------
Cell::Cell(float Left, float Bottom, float Width, float Height)
{
	BoundingBox.Min = { Left, Bottom };
	BoundingBox.Max = { BoundingBox.Min.X + Width, BoundingBox.Min.Y + Height };
}

std::vector<FVector2D> Cell::GetRectPoints() const
{
	const float left = BoundingBox.Min.X;
	const float bottom = BoundingBox.Min.Y;
	const float width = BoundingBox.Max.X - BoundingBox.Min.X;
	const float height = BoundingBox.Max.Y - BoundingBox.Min.Y;

	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Rows, int Cols, int MaxEntities)
	: pWorld{pWorld}
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, NrOfRows{Rows}
	, NrOfCols{Cols}
	, NrOfNeighbors{0}
{
	Neighbors.SetNum(MaxEntities);
	
	//calculate bounds of a cell
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;
	
	CellOrigin = { -Width / 2.f, -Height / 2.f };
	
	// TODO create the cells
	Cells.reserve(Rows * Cols);
	for (int Row = 0; Row < Rows; ++Row)
	{
		for (int Col = 0; Col < Cols; ++Col)
		{
			float Left   = CellOrigin.X + Col * CellWidth;
			float Bottom = CellOrigin.Y + Row * CellHeight;
			Cells.push_back(Cell(Left, Bottom, CellWidth, CellHeight));
		}
	}
}

void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	if (&Agent == nullptr) return;  
	// TODO Add the agent to the correct cell
	int Index{PositionToIndex(Agent.GetPosition())};
	Cells[Index].Agents.emplace_back(&Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent, const FVector2D& OldPos)
{
	//TODO Check if the agent needs to be moved to another cell.
	//TODO Use the calculated index for oldPos and currentPos for this
	if (&Agent == nullptr) return;  
	int NewIndex{PositionToIndex(Agent.GetPosition())};
	int OldIndex{PositionToIndex(OldPos)};
	
	if (NewIndex != OldIndex)
	{
		Cells[OldIndex].Agents.remove(&Agent);
		Cells[NewIndex].Agents.emplace_back(&Agent);
	}
}

void CellSpace::RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius)
{
	// TODO Register the neighbors for the provided agent
	// TODO Only check the cells that are within the radius of the neighborhood
	NrOfNeighbors = 0;

	FRect QueryRect;
	QueryRect.Min = Agent.GetPosition() - FVector2D(QueryRadius, QueryRadius);
	QueryRect.Max = Agent.GetPosition() + FVector2D(QueryRadius, QueryRadius);

	for (const Cell& cell : Cells)
	{
		if (!DoRectsOverlap(cell.BoundingBox, QueryRect)) continue; // not close enough

		for (ASteeringAgent* pAgent : cell.Agents)
		{
			if (pAgent != &Agent && pAgent != nullptr)
			{
				Neighbors[NrOfNeighbors++] = pAgent;
				if(GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Nr Neighbors: %d"), NrOfNeighbors));
			}
		}
	}
}

void CellSpace::EmptyCells()
{
	for (Cell& c : Cells)
		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	// TODO Render the cells with the number of agents inside of it
}

int CellSpace::PositionToIndex(FVector2D const & Pos) const
{
	// TODO Calculate the index of the cell based on the position
	FVector2D LocalPos = Pos - CellOrigin; // [0, WorldSize*2]
    
	int Row = FMath::Clamp((int)(LocalPos.Y / CellHeight), 0, NrOfRows - 1);
	int Col = FMath::Clamp((int)(LocalPos.X / CellWidth), 0, NrOfCols - 1);
    
	return Row * NrOfCols + Col;
}

bool CellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	// Check if the rectangles are separated on either axis
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	// If they are not separated, they must overlap
	return true;
}